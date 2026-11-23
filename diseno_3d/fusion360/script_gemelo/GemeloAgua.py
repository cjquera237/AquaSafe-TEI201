# -*- coding: utf-8 -*-
"""
Gemelo 3D - Monitor IoT de calidad de agua (TEI201 Avance #3)

Replica el prototipo real:
  - Caja estanca comercial 200 x 155 x 80 mm (base 74 + tapa 4), esquinas
    redondeadas, torres de tornillo en las esquinas y 6 prensaestopas conicos
    (2 por pared larga, 1 por pared corta). Tapa atornillada (4 tornillos).
  - Placa base de acrilico 175 x 116 x 3 mm sobre 4 torres, con todos los
    modulos montados como en la foto del prototipo:
      power bank 2x18650 (arriba izq.), adaptador DS18B20 (arriba centro),
      modulo pH PH-4502C con BNC hacia la izquierda (abajo izq.),
      adaptador de turbidez (abajo centro), shield + ESP32 30 pines con USB-C
      hacia abajo (derecha) y pantalla OLED 0.96" de pie en el borde derecho.
  - Sondas externas (pH con BNC, turbidez, DS18B20) saliendo por prensaestopas.

Modelos reales importados (carpeta componentes): ESP32 30p, shield ESP32,
power bank 2x18650 (STL) y OLED 0.96". pH / turbidez / DS18B20 se modelan
parametricamente con medidas de fichas tecnicas.
"""
import adsk.core, adsk.fusion, traceback, os

MM = 0.1  # Fusion trabaja internamente en cm
COMP_DIR = os.path.join(os.path.expanduser('~'), 'Documents', 'GemeloAgua', 'componentes')

# ---------------- caja estanca (mm) ----------------
LX, WY = 200.0, 155.0      # exterior
HB = 74.0                  # alto base (exterior)
LID_T = 4.0                # tapa
WALL = 3.0
R_EXT = 10.0               # radio esquinas
L_IN, W_IN, H_IN = LX - 2 * WALL, WY - 2 * WALL, HB - WALL
GLAND_R = 10.0             # prensaestopas M20 (agujero D20)
GLAND_Z = 40.0
CORNER = [(7.0, 7.0), (L_IN - 7.0, 7.0), (7.0, W_IN - 7.0), (L_IN - 7.0, W_IN - 7.0)]

# ---------------- placa acrilico y modulos (coords de la placa, mm) ----------------
PL_X, PL_Y, PL_L, PL_W, PL_T = 9.5, 16.5, 175.0, 116.0, 3.0
BOSS_H = 6.0
ZP = WALL + BOSS_H + PL_T          # cara superior de la placa (abs)
SP_H = 3.0                          # separadores de nylon bajo cada modulo
ZB = ZP + SP_H                      # cara inferior de los modulos

PB = (5.0, 52.0)                    # power bank (STL 100.6 x 49.9 x 21.4)
DS = (62.0, 103.0, 26.0, 11.0)      # adaptador DS18B20 (rojo)
PH = (28.0, 5.0, 41.05, 31.12, 1.55)  # PH-4502C
TB = (74.0, 5.0, 38.0, 28.0)        # adaptador turbidez (azul)
SH = (120.0, 20.0, 48.0, 56.0)      # shield ESP32 (STEP medido)
OL = (170.5, 38.0)                  # OLED de pie


def P(x, y, z):
    return adsk.core.Point3D.create(x * MM, y * MM, z * MM)


def vi(mm):
    return adsk.core.ValueInput.createByReal(mm * MM)


def I(v):
    """Coordenada interior de la caja -> modelo."""
    return WALL + v


def Q(px):
    """Coordenada X de la placa -> modelo."""
    return I(PL_X + px)


def R(py):
    """Coordenada Y de la placa -> modelo."""
    return I(PL_Y + py)


NEW = adsk.fusion.FeatureOperations.NewBodyFeatureOperation
JOIN = adsk.fusion.FeatureOperations.JoinFeatureOperation
CUT = adsk.fusion.FeatureOperations.CutFeatureOperation

PENDING = []
BUILD_LOG = []


def log_line(s):
    BUILD_LOG.append(str(s))


def _normal_comp(plane, axis):
    try:
        n = plane.geometry.normal
        return {'x': n.x, 'y': n.y, 'z': n.z}[axis]
    except Exception:
        return 1.0


def plane_at(comp, base_plane, axis, coord_mm):
    """Plano paralelo a base_plane que pasa por coord (en el eje dado)."""
    if abs(coord_mm) < 1e-9:
        return base_plane
    s = 1.0 if _normal_comp(base_plane, axis) >= 0 else -1.0
    pin = comp.constructionPlanes.createInput()
    pin.setByOffset(base_plane, vi(coord_mm * s))
    return comp.constructionPlanes.add(pin)


def _name(ext, name):
    try:
        if name and ext.bodies.count > 0:
            ext.bodies.item(0).name = name
            return ext.bodies.item(0)
    except Exception:
        pass
    return None


def _extrude(comp, sk, dist_mm, op, name):
    ext = comp.features.extrudeFeatures.addSimple(sk.profiles.item(0), vi(dist_mm), op)
    return _name(ext, name)


def box(comp, name, x, y, z, dx, dy, dz, op=NEW):
    pl = plane_at(comp, comp.xYConstructionPlane, 'z', z)
    sk = comp.sketches.add(pl)
    sk.sketchCurves.sketchLines.addTwoPointRectangle(
        sk.modelToSketchSpace(P(x, y, z)), sk.modelToSketchSpace(P(x + dx, y + dy, z)))
    s = 1.0 if _normal_comp(pl, 'z') >= 0 else -1.0
    return _extrude(comp, sk, dz * s, op, name)


def cyl_z(comp, name, cx, cy, z, r, h, op=NEW):
    pl = plane_at(comp, comp.xYConstructionPlane, 'z', z)
    sk = comp.sketches.add(pl)
    sk.sketchCurves.sketchCircles.addByCenterRadius(sk.modelToSketchSpace(P(cx, cy, z)), r * MM)
    s = 1.0 if _normal_comp(pl, 'z') >= 0 else -1.0
    return _extrude(comp, sk, h * s, op, name)


def cyl_x(comp, name, x, cy, cz, r, length, op=NEW):
    pl = plane_at(comp, comp.yZConstructionPlane, 'x', x)
    sk = comp.sketches.add(pl)
    sk.sketchCurves.sketchCircles.addByCenterRadius(sk.modelToSketchSpace(P(x, cy, cz)), r * MM)
    s = 1.0 if _normal_comp(pl, 'x') >= 0 else -1.0
    return _extrude(comp, sk, length * s, op, name)


def cyl_y(comp, name, cx, y, cz, r, length, op=NEW):
    pl = plane_at(comp, comp.xZConstructionPlane, 'y', y)
    sk = comp.sketches.add(pl)
    sk.sketchCurves.sketchCircles.addByCenterRadius(sk.modelToSketchSpace(P(cx, y, cz)), r * MM)
    s = 1.0 if _normal_comp(pl, 'y') >= 0 else -1.0
    return _extrude(comp, sk, length * s, op, name)


def new_comp(root, name):
    occ = root.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    occ.component.name = name
    return occ.component


# ---------------- apariencias ----------------
_app_cache = {}


def make_appearance(app, design, name, rgb):
    if name in _app_cache:
        return _app_cache[name]
    try:
        existing = design.appearances.itemByName(name)
        if existing:
            _app_cache[name] = existing
            return existing
        base = None
        fallback = None
        for i in range(app.materialLibraries.count):
            lib = app.materialLibraries.item(i)
            try:
                n = lib.appearances.count
            except Exception:
                continue
            for j in range(n):
                a = lib.appearances.item(j)
                nm = a.name.lower()
                if fallback is None:
                    fallback = a
                if ('plastic' in nm or u'plástico' in nm or 'plastico' in nm) \
                        and ('matte' in nm or 'mate' in nm or 'glossy' in nm or 'brillante' in nm):
                    base = a
                    break
                if base is None and ('plastic' in nm or u'plástico' in nm):
                    base = a
            if base:
                break
        base = base or fallback
        if not base:
            return None
        ap = design.appearances.addByCopy(base, name)
        for k in range(ap.appearanceProperties.count):
            prop = ap.appearanceProperties.item(k)
            if prop.objectType == adsk.core.ColorProperty.classType():
                prop.value = adsk.core.Color.create(rgb[0], rgb[1], rgb[2], 255)
                break
        _app_cache[name] = ap
        return ap
    except Exception:
        return None


def paint(app, design, comp, name, rgb, key=None):
    ap = make_appearance(app, design, name, rgb)
    if not ap:
        return
    for i in range(comp.bRepBodies.count):
        b = comp.bRepBodies.item(i)
        if key is None or key in b.name:
            try:
                b.appearance = ap
            except Exception:
                pass


def add_params(design):
    up = design.userParameters
    for n, v, c in (('L_ext', LX, 'Largo exterior caja'), ('W_ext', WY, 'Ancho exterior caja'),
                    ('H_base', HB, 'Alto base'), ('wall', WALL, 'Espesor de pared'),
                    ('lid_t', LID_T, 'Espesor tapa'), ('r_esq', R_EXT, 'Radio esquinas'),
                    ('d_gland', 2 * GLAND_R, 'Diametro prensaestopas')):
        if up.itemByName(n) is None:
            up.add(n, adsk.core.ValueInput.createByString('%g mm' % v), 'mm', c)


# ---------------- caja ----------------
def _vertical_edges(body):
    coll = adsk.core.ObjectCollection.create()
    for i in range(body.edges.count):
        e = body.edges.item(i)
        g = e.geometry
        if g.objectType == adsk.core.Line3D.classType():
            s, t = g.startPoint, g.endPoint
            if abs(s.x - t.x) < 1e-6 and abs(s.y - t.y) < 1e-6:
                coll.add(e)
    return coll


def _fillet(comp, edges, r_mm):
    fi = comp.features.filletFeatures.createInput()
    try:
        fi.edgeSetInputs.addConstantRadiusEdgeSet(edges, vi(r_mm), True)
    except Exception:
        fi.addConstantRadiusEdgeSet(edges, vi(r_mm), True)
    comp.features.filletFeatures.add(fi)


def _top_face(body):
    best = None
    for i in range(body.faces.count):
        f = body.faces.item(i)
        try:
            ok, n = f.evaluator.getNormalAtPoint(f.pointOnFace)
            if ok and n.z > 0.99:
                z = f.pointOnFace.z
                if best is None or z > best[0]:
                    best = (z, f)
        except Exception:
            pass
    return best[1] if best else None


def gland_positions():
    """(pared, centro) de los 6 prensaestopas."""
    return [('front', I(58.0)), ('front', I(136.0)),
            ('back', I(58.0)), ('back', I(136.0)),
            ('left', I(W_IN / 2)), ('right', I(W_IN / 2))]


def build_base(root):
    base = new_comp(root, 'Carcasa_Base')
    sk = base.sketches.add(base.xYConstructionPlane)
    sk.name = 'sk_contorno'
    sk.sketchCurves.sketchLines.addTwoPointRectangle(P(0, 0, 0), P(LX, WY, 0))
    ext = base.features.extrudeFeatures.addSimple(sk.profiles.item(0), vi(HB), NEW)
    body = ext.bodies.item(0)
    body.name = 'Base'
    _fillet(base, _vertical_edges(body), R_EXT)
    body = base.bRepBodies.item(0)
    faces = adsk.core.ObjectCollection.create()
    faces.add(_top_face(body))
    sin = base.features.shellFeatures.createInput(faces, False)
    sin.insideThickness = vi(WALL)
    base.features.shellFeatures.add(sin)
    body = base.bRepBodies.item(0)

    # Torres de esquina para los tornillos de la tapa (+ agujero D3.5 x 15)
    for (cx, cy) in CORNER:
        cyl_z(base, None, I(cx), I(cy), WALL, 8.0, H_IN, JOIN)
    for (cx, cy) in CORNER:
        cyl_z(base, None, I(cx), I(cy), HB, 1.75, -15, CUT)
    # Torres para la placa de acrilico (+ agujero D2.5)
    for (px, py) in ((6, 6), (PL_L - 6, 6), (6, PL_W - 6), (PL_L - 6, PL_W - 6)):
        cyl_z(base, None, Q(px), R(py), WALL, 4.0, BOSS_H, JOIN)
        cyl_z(base, None, Q(px), R(py), WALL + BOSS_H, 1.25, -BOSS_H, CUT)

    # Agujeros de los 6 prensaestopas (D20)
    for wall, c in gland_positions():
        if wall == 'front':
            cyl_y(base, None, c, -1.0, GLAND_Z, GLAND_R, WALL + 2.0, CUT)
        elif wall == 'back':
            cyl_y(base, None, c, WY + 1.0, GLAND_Z, GLAND_R, -(WALL + 2.0), CUT)
        elif wall == 'left':
            cyl_x(base, None, -1.0, c, GLAND_Z, GLAND_R, WALL + 2.0, CUT)
        else:
            cyl_x(base, None, LX + 1.0, c, GLAND_Z, GLAND_R, -(WALL + 2.0), CUT)
    return base


def build_lid(root):
    lid = new_comp(root, 'Tapa')
    pl = plane_at(lid, lid.xYConstructionPlane, 'z', HB)
    sk = lid.sketches.add(pl)
    sk.sketchCurves.sketchLines.addTwoPointRectangle(
        sk.modelToSketchSpace(P(0, 0, HB)), sk.modelToSketchSpace(P(LX, WY, HB)))
    ext = lid.features.extrudeFeatures.addSimple(sk.profiles.item(0), vi(LID_T), NEW)
    ext.bodies.item(0).name = 'Tapa'
    _fillet(lid, _vertical_edges(lid.bRepBodies.item(0)), R_EXT)
    for (cx, cy) in CORNER:
        cyl_z(lid, None, I(cx), I(cy), HB, 2.0, LID_T, CUT)
        cyl_z(lid, None, I(cx), I(cy), HB + LID_T, 3.5, -2.0, CUT)
    return lid


CABLES = {}   # indice prensaestopas -> radio del cable que pasa


def build_glands(root):
    """Prensaestopas conicos (goma): tubo dentro del agujero + ala exterior.
    Los que llevan cable tienen el agujero del cable perforado."""
    c = new_comp(root, 'Prensaestopas')
    for k, (wall, ctr) in enumerate(gland_positions()):
        nm = 'Prensaestopa_%d' % (k + 1)
        rc = CABLES.get(k)
        if wall in ('front', 'back'):
            y0, d = (0.0, WALL) if wall == 'front' else (WY, -WALL)
            out = -2.0 if wall == 'front' else 2.0
            cyl_y(c, nm, ctr, y0 + d, GLAND_Z, GLAND_R, -d)
            cyl_y(c, nm + '_ala', ctr, y0, GLAND_Z, 12.5, out)
            if rc:
                cyl_y(c, None, ctr, y0 + d + (1 if d > 0 else -1), GLAND_Z, rc,
                      -(d + (1 if d > 0 else -1)) + out * 1.5, CUT)
        else:
            x0, d = (0.0, WALL) if wall == 'left' else (LX, -WALL)
            out = -2.0 if wall == 'left' else 2.0
            cyl_x(c, nm, x0 + d, ctr, GLAND_Z, GLAND_R, -d)
            cyl_x(c, nm + '_ala', x0, ctr, GLAND_Z, 12.5, out)
    return c


# ---------------- modelos importados ----------------
def _mat_axes(t_mm, xaxis, yaxis, zaxis):
    m = adsk.core.Matrix3D.create()
    m.setWithCoordinateSystem(adsk.core.Point3D.create(t_mm[0] * MM, t_mm[1] * MM, t_mm[2] * MM),
                              adsk.core.Vector3D.create(*xaxis),
                              adsk.core.Vector3D.create(*yaxis),
                              adsk.core.Vector3D.create(*zaxis))
    return m


def place_by_bbox(nmin, nmax, perm, target_min, signs=(1, 1, 1)):
    """Matriz que lleva el modelo nativo a: eje mundo i <- signs[i] * eje nativo
    perm[i] (rotacion propia), con el minimo del bbox en target_min."""
    axes = [[0, 0, 0], [0, 0, 0], [0, 0, 0]]   # imagen de cada eje nativo
    for i in range(3):
        axes[perm[i]][i] = signs[i]
    t = [target_min[i] - (nmin[perm[i]] if signs[i] > 0 else -nmax[perm[i]])
         for i in range(3)]
    return _mat_axes(t, axes[0], axes[1], axes[2])


def import_step(app, root, fname, name):
    n0 = root.occurrences.count
    opt = app.importManager.createSTEPImportOptions(os.path.join(COMP_DIR, fname))
    opt.isViewFit = False
    app.importManager.importToTarget(opt, root)
    occ = root.occurrences.item(n0)
    try:
        occ.component.name = name
    except Exception:
        pass
    return occ


def _bb_mm(b):
    return ([b.minPoint.x * 10, b.minPoint.y * 10, b.minPoint.z * 10],
            [b.maxPoint.x * 10, b.maxPoint.y * 10, b.maxPoint.z * 10])


def _usb_bb(esp):
    comp = esp.component
    for i in range(comp.allOccurrences.count):
        c = comp.allOccurrences.item(i)
        if 'usb' in c.name.lower():
            return _bb_mm(c.boundingBox)
    best = None
    for i in range(comp.bRepBodies.count):
        b = comp.bRepBodies.item(i).boundingBox
        sx = (b.maxPoint.x - b.minPoint.x) * 10
        sz = (b.maxPoint.z - b.minPoint.z) * 10
        if 7.0 < sx < 10.5 and 2.0 < sz < 4.5:
            ext = max(-b.minPoint.y, b.maxPoint.y) * 10
            if best is None or ext > best[0]:
                best = (ext, b)
    return _bb_mm(best[1]) if best else None


def build_esp_and_shield(app, root):
    """Shield (EasyEDA 48x56) + ESP32 WROOM 30 pines USB-C insertado en los
    zocalos U1/U2. USB-C hacia el borde inferior de la placa (como la foto)."""
    sh = import_step(app, root, 'shield_esp32.step', 'Shield_ESP32')
    esp = import_step(app, root, 'esp32_30p.step', 'ESP32_DevKit_30p')
    t_e = (24.0, -29.7 + 0.85, 9.3 + 2.5)
    usb = _usb_bb(esp)
    usb_y = ((usb[0][1] + usb[1][1]) / 2) if usb else -27.0
    log_line('USB ESP32 (coords modelo): %s' % (usb,))
    x0, y0, w, h = SH
    if usb_y + t_e[1] < -28.0:      # USB hacia el borde nativo y=-56 -> sin giro
        m_s = _mat_axes((Q(x0), R(y0 + h), ZB), (1, 0, 0), (0, 1, 0), (0, 0, 1))
    else:
        m_s = _mat_axes((Q(x0 + w), R(y0), ZB), (-1, 0, 0), (0, -1, 0), (0, 0, 1))
    m_e = _mat_axes(t_e, (1, 0, 0), (0, 1, 0), (0, 0, 1))
    m_e.transformBy(m_s)
    PENDING.append((sh, m_s))
    PENDING.append((esp, m_e))
    return sh, esp


def build_oled(app, root):
    """OLED 0.96" SSD1306 de pie en el borde derecho de la placa, pantalla
    hacia +X (eje delgado nativo Z -> X mundo)."""
    o = import_step(app, root, 'oled_096.step', 'OLED_096')
    nmin, nmax = _bb_mm(o.boundingBox)
    log_line('OLED bbox nativo: %s %s' % (nmin, nmax))
    PENDING.append((o, place_by_bbox(nmin, nmax, (2, 0, 1), (Q(OL[0]), R(OL[1]), ZB))))
    return o


def build_powerbank(root):
    """Modulo power bank 2x18650 con salida USB (STL real, 49.9 x 21.4 x 100.6)."""
    occ = root.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    c = occ.component
    c.name = 'PowerBank_2x18650'
    bf = c.features.baseFeatures.add()
    bf.startEdit()
    try:
        c.meshBodies.add(os.path.join(COMP_DIR, 'powerbank_2x18650.stl'),
                         adsk.fusion.MeshUnits.MillimeterMeshUnit, bf)
    finally:
        bf.finishEdit()
    smin = (17.941, 55.482, 18.617)
    smax = (67.853, 76.926, 119.190)
    # girado 180 en Z: salida USB-A hacia la izquierda (-X), como el prototipo
    PENDING.append((occ, place_by_bbox(smin, smax, (2, 0, 1), (Q(PB[0]), R(PB[1]), ZP),
                                       (-1, -1, 1))))
    return occ


def build_cells(root):
    """Dos pilas 18650 (18.5 x 65 mm) dentro del portapilas."""
    c = new_comp(root, 'Pilas_18650')
    yc = R(PB[1] + 49.9 / 2)
    for k, dy in enumerate((-11.0, 11.0)):
        cyl_x(c, 'Pila_18650_%d' % (k + 1), Q(PB[0] + 27.5), yc + dy, ZP + 3.0 + 9.25,
              9.25, 65.0)
    return c


# ---------------- placa y modulos parametricos ----------------
def build_plate(root):
    c = new_comp(root, 'Placa_acrilico')
    box(c, 'Placa_acrilico_3mm', Q(0), R(0), WALL + BOSS_H, PL_L, PL_W, PL_T)
    for (px, py) in ((6, 6), (PL_L - 6, 6), (6, PL_W - 6), (PL_L - 6, PL_W - 6)):
        cyl_z(c, None, Q(px), R(py), WALL + BOSS_H, 1.6, PL_T, CUT)
    # separadores de nylon bajo cada modulo (3 mm)
    spots = []
    for (x, y, w, h) in ((PH[0], PH[1], PH[2], PH[3]), TB, DS, SH):
        spots += [(x + 3, y + 3), (x + w - 3, y + 3), (x + 3, y + h - 3), (x + w - 3, y + h - 3)]
    for k, (px, py) in enumerate(spots):
        cyl_z(c, 'Separador_%d' % (k + 1), Q(px), R(py), ZP, 2.0, SP_H)
    return c


def build_ph(root):
    """PH-4502C (similar al modelo GrabCAD): placa 41.05 x 31.12 x 1.55,
    BNC hembra en angulo hacia -X (eje 6.7 mm sobre la placa), trimmers 3296W."""
    c = new_comp(root, 'Modulo_pH_PH4502C')
    px, py, L, W, T = PH
    x0, y0 = Q(px), R(py)
    cy = y0 + W / 2
    box(c, 'PCB_pH', x0, y0, ZB, L, W, T)
    zt = ZB + T
    zc = zt + 6.7
    box(c, 'BNC_cuerpo', x0 - 3.5, cy - 7.25, zt, 16.0, 14.5, 13.4)
    cyl_x(c, 'BNC_brida', x0 - 3.5, cy, zc, 7.3, -3.5)
    cyl_x(c, 'BNC_rosca', x0 - 7.0, cy, zc, 5.875, -10.0)
    cyl_x(c, None, x0 - 10.0, cy, zc, 8.47, -2.5, JOIN)     # tuerca
    cyl_x(c, 'BNC_macho_sonda', x0 - 17.0, cy, zc, 7.0, -14.0)
    for k, xx in enumerate((x0 + 16.0, x0 + 27.0)):
        box(c, 'Trimmer_3296W_%d' % (k + 1), xx, y0 + W - 6.0, zt, 9.53, 4.83, 10.03)
    for k in range(3):
        box(c, 'SOIC8_%d' % (k + 1), x0 + 16.0 + k * 8.0, y0 + 4.0, zt, 5.0, 6.0, 1.75)
    box(c, 'Header_6p', x0 + L - 2.5, y0 + 8.0, zt, 2.5, 15.24, 2.5)
    return c


def build_turb(root):
    """Adaptador del sensor de turbidez (placa azul, 2 conectores JST)."""
    c = new_comp(root, 'Modulo_Turbidez')
    px, py, L, W = TB
    x0, y0 = Q(px), R(py)
    box(c, 'PCB_turbidez', x0, y0, ZB, L, W, 1.6)
    zt = ZB + 1.6
    box(c, 'JST_sonda', x0 + 1, y0 + 4, zt, 6, 8, 6)
    box(c, 'JST_salida', x0 + 1, y0 + 15, zt, 6, 8, 6)
    box(c, 'SOIC_1', x0 + 14, y0 + 6, zt, 5, 6, 1.75)
    box(c, 'SOIC_2', x0 + 24, y0 + 6, zt, 5, 6, 1.75)
    box(c, 'Trimmer', x0 + 26, y0 + 17, zt, 7, 7, 5)
    return c


def build_ds(root):
    """Adaptador DS18B20 (placa roja con pull-up, JST y bornera)."""
    c = new_comp(root, 'Modulo_DS18B20')
    px, py, L, W = DS
    x0, y0 = Q(px), R(py)
    box(c, 'PCB_DS18B20', x0, y0, ZB, L, W, 1.6)
    zt = ZB + 1.6
    box(c, 'JST_3p', x0 + 2, y0 + 2, zt, 8, 6, 6)
    box(c, 'Bornera_3p', x0 + 14, y0 + 1.5, zt, 10, 7.5, 9)
    return c


def build_probes(root):
    """Sondas externas, cada cable sale por un prensaestopas."""
    c = new_comp(root, 'Sondas_externas')
    gp = gland_positions()
    # pH (electrodo E-201-C) por prensaestopas 1 (frontal)
    xg = gp[0][1]
    cyl_y(c, 'Cable_pH_h', xg, WALL + 8.0, GLAND_Z, 2.5, -(WALL + 8.0 + 25.0))
    cyl_z(c, 'Cable_pH_v', xg, -27.5, 10.0, 2.5, GLAND_Z + 2.5 - 10.0)
    cyl_z(c, 'Sonda_pH_cabeza', xg, -27.5, -20.0, 9.0, 30.0)
    cyl_z(c, 'Sonda_pH_cuerpo', xg, -27.5, -150.0, 6.0, 130.0)
    CABLES[0] = 2.5
    # Turbidez por prensaestopas 2 (frontal)
    xg = gp[1][1]
    cyl_y(c, 'Cable_turb_h', xg, WALL + 8.0, GLAND_Z, 2.5, -(WALL + 8.0 + 30.0))
    cyl_z(c, 'Cable_turb_v', xg, -32.5, -10.0, 2.5, GLAND_Z + 2.5 + 10.0)
    cyl_z(c, 'Sonda_turbidez_cabezal', xg, -32.5, -35.0, 15.0, 25.0)
    box(c, 'Sonda_turbidez_emisor', xg - 14, -32.5 - 9, -60.0, 10, 18, 25)
    box(c, 'Sonda_turbidez_receptor', xg + 4, -32.5 - 9, -60.0, 10, 18, 25)
    CABLES[1] = 2.5
    # DS18B20 por prensaestopas 3 (trasero)
    xg = gp[2][1]
    cyl_y(c, 'Cable_DS_h', xg, WY - WALL - 8.0, GLAND_Z, 2.0, WALL + 8.0 + 18.0)
    cyl_z(c, 'Cable_DS_v', xg, WY + 20.0, -10.0, 2.0, GLAND_Z + 2.0 + 10.0)
    cyl_z(c, 'DS18B20_vaina', xg, WY + 20.0, -60.0, 3.0, 50.0)
    CABLES[2] = 2.0
    return c


# ---------------- interferencias ----------------
def check_interference(design, root):
    lines = []
    try:
        bodies = adsk.core.ObjectCollection.create()
        for i in range(root.allOccurrences.count):
            occ = root.allOccurrences.item(i)
            for j in range(occ.bRepBodies.count):
                bodies.add(occ.bRepBodies.item(j))
        inp = design.createInterferenceInput(bodies)
        inp.areCoincidentFacesIncluded = False
        res = design.analyzeInterference(inp)
        owner = {}
        for i in range(root.occurrences.count):
            o = root.occurrences.item(i)
            owner[o.component.name] = o.component.name
            for j in range(o.component.allOccurrences.count):
                owner[o.component.allOccurrences.item(j).component.name] = o.component.name

        def top(e):
            try:
                if e.assemblyContext:
                    return e.assemblyContext.fullPathName.split('+')[0].split(':')[0]
            except Exception:
                pass
            try:
                return owner.get(e.parentComponent.name, e.parentComponent.name)
            except Exception:
                return ''
        acople = ('ESP32_DevKit_30p', 'Shield_ESP32')
        reales = intencional = 0
        for k in range(res.count):
            r = res.item(k)
            a, b = r.entityOne, r.entityTwo
            ta, tb = top(a), top(b)
            try:
                vol = r.interferenceBody.volume * 1000.0
            except Exception:
                vol = 0.0
            if ta == tb or (ta in acople and tb in acople):
                intencional += 1
                tag = 'ACOPLE/INTERNO'
            else:
                reales += 1
                tag = 'REVISAR'
            lines.append('[%s] %s/%s <-> %s/%s : %.3f mm3' % (tag, ta, a.name, tb, b.name, vol))
        head = ('Interferencias entre componentes: %d | acoples pin-zocalo ESP32/shield '
                'o internos de un modelo comprado: %d (cuerpos analizados: %d)'
                % (reales, intencional, bodies.count))
    except Exception:
        head = 'No se pudo analizar interferencias:\n' + traceback.format_exc()
    lines.sort(key=lambda s: not s.startswith('[REVISAR'))
    txt = head + '\n' + '\n'.join(BUILD_LOG) + '\n' + '\n'.join(lines)
    try:
        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                               'interferencias.txt'), 'w', encoding='utf-8') as f:
            f.write(txt)
    except Exception:
        pass
    return head


def run(context):
    ui = None
    try:
        del PENDING[:]
        del BUILD_LOG[:]
        CABLES.clear()
        app = adsk.core.Application.get()
        ui = app.userInterface
        app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        design = adsk.fusion.Design.cast(app.activeProduct)
        design.designType = adsk.fusion.DesignTypes.ParametricDesignType
        root = design.rootComponent
        add_params(design)

        base = build_base(root)
        lid = build_lid(root)
        plate = build_plate(root)
        ph = build_ph(root)
        turb = build_turb(root)
        ds = build_ds(root)
        probes = build_probes(root)
        glands = build_glands(root)
        cells = build_cells(root)
        bat = build_powerbank(root)
        shield, esp = build_esp_and_shield(app, root)
        oled = build_oled(app, root)

        for occ, m in PENDING:
            occ.transform2 = m
        try:
            design.snapshots.add()
        except Exception:
            pass

        paint(app, design, base, 'TEI_Caja_Gris', (225, 228, 230))
        paint(app, design, lid, 'TEI_Caja_Gris', (225, 228, 230))
        paint(app, design, glands, 'TEI_Goma_Crema', (235, 222, 180))
        paint(app, design, plate, 'TEI_Acrilico', (205, 225, 235), 'Placa')
        paint(app, design, plate, 'TEI_Nylon', (245, 245, 240), 'Separador')
        paint(app, design, ph, 'TEI_PCB_Verde', (20, 120, 60))
        paint(app, design, turb, 'TEI_PCB_Azul', (30, 70, 170))
        paint(app, design, ds, 'TEI_PCB_Rojo', (200, 40, 40))
        paint(app, design, probes, 'TEI_Sonda', (35, 35, 35))
        paint(app, design, ph, 'TEI_Metal', (190, 190, 195), 'BNC')
        paint(app, design, ph, 'TEI_Trimmer', (40, 90, 210), 'Trimmer')
        paint(app, design, ph, 'TEI_Negro', (25, 25, 25), 'SOIC')
        paint(app, design, turb, 'TEI_JST', (245, 245, 240), 'JST')
        paint(app, design, ds, 'TEI_JST', (245, 245, 240), 'JST')
        paint(app, design, ds, 'TEI_Bornera', (30, 140, 70), 'Bornera')
        paint(app, design, probes, 'TEI_Vidrio', (200, 215, 220), 'cuerpo')
        paint(app, design, probes, 'TEI_Inox', (190, 190, 195), 'vaina')
        paint(app, design, cells, 'TEI_Pila_Roja', (200, 35, 35))
        try:
            ap = make_appearance(app, design, 'TEI_PowerBank', (25, 25, 25))
            for i in range(bat.component.meshBodies.count):
                bat.component.meshBodies.item(i).appearance = ap
        except Exception:
            pass

        app.activeViewport.fit()
        report = check_interference(design, root)
        ui.messageBox('Gemelo 3D creado (caja estanca 200x155x80 + placa de acrilico).\n\n'
                      + report)
    except Exception:
        if ui:
            ui.messageBox('Error:\n{}'.format(traceback.format_exc()))
