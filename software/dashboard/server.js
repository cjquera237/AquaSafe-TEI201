
require("dotenv").config();

const express = require("express");
const path = require("path");
const { Pool } = require("pg");

const app = express();
app.use(express.json({ limit: "16kb" }));
app.use(express.static(path.join(__dirname, "public")));

// ========================================
// AQUASAFE - SERVIDOR Y BASE DE DATOS
// ========================================

const databaseURL = process.env.DATABASE_URL;
const certificado = process.env.DB_CA_CERT;

if (!databaseURL || !certificado || !process.env.API_KEY) {
  console.error(
    "Faltan DATABASE_URL, DB_CA_CERT o API_KEY en Railway."
  );
  process.exit(1);
}

const pool = new Pool({
  connectionString: databaseURL.split("?")[0],
  ssl: {
    ca: certificado.replace(/\\n/g, "\n"),
    rejectUnauthorized: true
  },
  max: 5
});

// ========================================
// CREAR O ACTUALIZAR TABLA
// ========================================

async function prepararBaseDeDatos() {
  await pool.query(`
    CREATE TABLE IF NOT EXISTS lecturas (
      id SERIAL PRIMARY KEY,
      dispositivo VARCHAR(50) NOT NULL,
      sensor VARCHAR(50) NOT NULL,
      valor DOUBLE PRECISION NOT NULL,
      unidad VARCHAR(10),
      creado_en TIMESTAMPTZ DEFAULT NOW()
    )
  `);

  // Actualiza la tabla antigua sin borrar datos.
  await pool.query(`
    ALTER TABLE lecturas
    ADD COLUMN IF NOT EXISTS unidad VARCHAR(10)
  `);

  console.log("Base de datos AquaSafe preparada.");
}

// ========================================
// SEGURIDAD DE LA API
// ========================================

function revisarClave(req, res, next) {
  const claveRecibida = req.get("x-api-key");

  if (claveRecibida !== process.env.API_KEY) {
    return res.status(401).json({
      error: "API_KEY invalida"
    });
  }

  next();
}

// ========================================
// UNIDADES PERMITIDAS
// ========================================

const unidadesPermitidas = {
  temperatura: ["°C"],
  ph: ["V", "pH"],
  turbidez: ["V", "UNT"],
  tds: ["V", "ppm"]
};

// ========================================
// COMPROBAR ESTADO DEL SERVIDOR
// ========================================

app.get("/api/salud", async (req, res) => {
  try {
    await pool.query("SELECT 1");

    res.json({
      ok: true,
      db: "conectada"
    });
  } catch (error) {
    console.error(error.message);

    res.status(500).json({
      ok: false,
      error: "No se pudo conectar a Aiven"
    });
  }
});

// ========================================
// RECIBIR MEDICIONES DEL ESP32
// ========================================

app.post("/api/lecturas", revisarClave, async (req, res) => {
  const {
    dispositivo,
    sensor,
    valor,
    unidad
  } = req.body || {};

  // Verificar datos recibidos.
  const valido =
    typeof dispositivo === "string" &&
    dispositivo.trim().length > 0 &&
    dispositivo.length <= 50 &&
    typeof sensor === "string" &&
    Object.prototype.hasOwnProperty.call(
      unidadesPermitidas,
      sensor
    ) &&
    typeof valor === "number" &&
    Number.isFinite(valor) &&
    typeof unidad === "string" &&
    unidadesPermitidas[sensor]?.includes(unidad);

  if (!valido) {
    return res.status(400).json({
      error: "Medicion invalida",
      ejemplo: {
        dispositivo: "aquasafe-esp32",
        sensor: "ph",
        valor: 0.18,
        unidad: "V"
      }
    });
  }

  try {
    const resultado = await pool.query(
      `INSERT INTO lecturas
        (dispositivo, sensor, valor, unidad)
       VALUES ($1, $2, $3, $4)
       RETURNING id`,
      [
        dispositivo.trim(),
        sensor,
        valor,
        unidad
      ]
    );

    res.status(201).json({
      ok: true,
      id: resultado.rows[0].id
    });
  } catch (error) {
    console.error("Error al guardar:", error.message);

    res.status(500).json({
      error: "No se pudo guardar la medicion"
    });
  }
});

// ========================================
// CONSULTAR HISTORIAL
// ========================================

app.get("/api/lecturas", async (req, res) => {
  const solicitado = Number.parseInt(
    req.query.limite,
    10
  );

  const limite = Number.isFinite(solicitado)
    ? Math.max(1, Math.min(solicitado, 500))
    : 100;

  try {
    const resultado = await pool.query(
      `SELECT
         id,
         dispositivo,
         sensor,
         valor,
         unidad,
         creado_en
       FROM lecturas
       ORDER BY id DESC
       LIMIT $1`,
      [limite]
    );

    res.json(resultado.rows);
  } catch (error) {
    console.error("Error al consultar:", error.message);

    res.status(500).json({
      error: "No se pudo obtener el historial"
    });
  }
});

// ========================================
// INICIAR SERVIDOR
// ========================================

const PORT = process.env.PORT || 3000;

async function iniciarServidor() {
  try {
    await prepararBaseDeDatos();

    app.listen(PORT, "0.0.0.0", () => {
      console.log(
        `AquaSafe iniciado en puerto ${PORT}`
      );
    });
  } catch (error) {
    console.error(
      "No se pudo iniciar AquaSafe:",
      error.message
    );
    process.exit(1);
  }
}

iniciarServidor();

