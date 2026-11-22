
require("dotenv").config();

const express = require("express");
const path = require("path");
const { Pool } = require("pg");

const app = express();

app.use(express.json());
app.use(express.static(path.join(__dirname, "public")));

// Conexion con PostgreSQL de Aiven
const pool = new Pool({
  connectionString: (process.env.DATABASE_URL || "").split("?")[0],
  ssl: process.env.DB_CA_CERT
    ? {
        ca: process.env.DB_CA_CERT.replace(/\\n/g, "\n"),
        rejectUnauthorized: true
      }
    : { rejectUnauthorized: false },
  max: 5
});

// Crear tabla para almacenar mediciones
async function crearTabla() {
  await pool.query(`
    CREATE TABLE IF NOT EXISTS lecturas (
      id SERIAL PRIMARY KEY,
      dispositivo VARCHAR(50) NOT NULL,
      sensor VARCHAR(50) NOT NULL,
      valor REAL NOT NULL,
      creado_en TIMESTAMPTZ DEFAULT NOW()
    )
  `);

  console.log("Tabla de lecturas lista");
}

// Verificar clave de acceso
function revisarClave(req, res, next) {
  if (!process.env.API_KEY ||
      req.get("x-api-key") !== process.env.API_KEY) {
    return res.status(401).json({
      error: "API key invalida"
    });
  }

  next();
}

// Comprobar estado del servidor
app.get("/api/salud", async (req, res) => {
  try {
    await pool.query("SELECT 1");
    res.json({ ok: true, db: "conectada" });
  } catch (error) {
    res.status(500).json({
      ok: false,
      error: "No se pudo conectar a la base de datos"
    });
  }
});

// Recibir mediciones del ESP32
app.post("/api/lecturas", revisarClave, async (req, res) => {
  const { dispositivo, sensor, valor } = req.body;

  const sensoresPermitidos = [
    "temperatura",
    "ph",
    "turbidez",
    "tds"
  ];

  if (
    typeof dispositivo !== "string" ||
    !dispositivo.trim() ||
    dispositivo.length > 50 ||
    typeof sensor !== "string" ||
    !sensoresPermitidos.includes(sensor) ||
    typeof valor !== "number" ||
    !Number.isFinite(valor)
  ) {
    return res.status(400).json({
      error: "Medicion invalida"
    });
  }

  try {
    const resultado = await pool.query(
      `INSERT INTO lecturas (dispositivo, sensor, valor)
       VALUES ($1, $2, $3)
       RETURNING id`,
      [dispositivo, sensor, valor]
    );

    res.status(201).json({
      ok: true,
      id: resultado.rows[0].id
    });
  } catch (error) {
    console.error(error);
    res.status(500).json({
      error: "No se pudo guardar la medicion"
    });
  }
});

// Consultar historial
app.get("/api/lecturas", async (req, res) => {
  const solicitado = Number.parseInt(req.query.limite, 10);
  const limite = Number.isFinite(solicitado)
    ? Math.max(1, Math.min(solicitado, 500))
    : 100;

  try {
    const resultado = await pool.query(
      `SELECT id, dispositivo, sensor, valor, creado_en
       FROM lecturas
       ORDER BY id DESC
       LIMIT $1`,
      [limite]
    );

    res.json(resultado.rows);
  } catch (error) {
    console.error(error);
    res.status(500).json({
      error: "No se pudieron consultar las lecturas"
    });
  }
});

// Iniciar servidor
const PORT = process.env.PORT || 3000;

app.listen(PORT, async () => {
  console.log(`Servidor iniciado en puerto ${PORT}`);

  try {
    await crearTabla();
  } catch (error) {
    console.error(
      "Error al preparar la base de datos:",
      error.message
    );
  }
});
