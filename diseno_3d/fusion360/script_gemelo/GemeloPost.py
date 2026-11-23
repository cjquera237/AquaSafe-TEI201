# -*- coding: utf-8 -*-
"""Post-proceso del gemelo 3D (TEI201 Avance #3):
exporta .f3d, renders fotorrealistas (espacio Render) e imagenes de vista."""
import adsk.core, adsk.fusion, traceback, os, time

OUT = os.path.join(os.path.expanduser('~'), 'Documents', 'GemeloAgua', u'diseño-3d')
REN = os.path.join(OUT, 'renders')
LOG = []


def log(s):
    LOG.append(s)


def find_occ(root, name):
    for i in range(root.occurrences.count):
        o = root.occurrences.item(i)
        if o.component.name == name:
            return o
    return None


def set_cam(app, eye, target, up=(0, 0, 1)):
    vp = app.activeViewport
    cam = vp.camera
    cam.isSmoothTransition = False
    cam.cameraType = adsk.core.CameraTypes.PerspectiveCameraType
    cam.eye = adsk.core.Point3D.create(*eye)
    cam.target = adsk.core.Point3D.create(*target)
    cam.upVector = adsk.core.Vector3D.create(*up)
    vp.camera = cam
    vp.fit()
    adsk.doEvents()
    return vp.camera


def snap(app, name):
    p = os.path.join(REN, name)
    ok = app.activeViewport.saveAsImageFile(p, 1920, 1080)
    log('imagen %s: %s' % (name, ok))


def render(app, design, name, cam):
    try:
        r = design.renderManager.rendering
        for attr, val in (('aspectRatio', 'Widescreen16x9AspectRatio'),
                          ('resolution', 'Widescreen1920x1080RenderResolution')):
            try:
                enum = adsk.fusion.RenderAspectRatios if attr == 'aspectRatio' \
                    else adsk.fusion.RenderResolutions
                setattr(r, attr, getattr(enum, val))
            except Exception:
                pass
        try:
            r.renderQuality = 60
        except Exception:
            pass
        p = os.path.join(REN, name)
        fut = r.startLocalRender(p, cam)
        t0 = time.time()
        while time.time() - t0 < 300:
            adsk.doEvents()
            st = fut.renderState
            if st in (adsk.fusion.LocalRenderStates.FinishedLocalRenderState,
                      adsk.fusion.LocalRenderStates.FailedLocalRenderState):
                break
            time.sleep(0.5)
        log('render %s: estado %s, existe=%s' % (name, fut.renderState, os.path.exists(p)))
    except Exception:
        log('render %s no disponible: %s' % (name, traceback.format_exc().splitlines()[-1]))


def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        os.makedirs(REN, exist_ok=True)
        doc = app.activeDocument
        design = adsk.fusion.Design.cast(doc.products.itemByProductType('DesignProductType'))
        root = design.rootComponent
        if find_occ(root, 'Carcasa_Base') is None:
            ui.messageBox('El documento activo no es el gemelo (falta Carcasa_Base).')
            return

        em = design.exportManager
        f3d = os.path.join(OUT, 'encapsulado.f3d')
        log('f3d: %s' % em.execute(em.createFusionArchiveExportOptions(f3d)))

        lid = find_occ(root, 'Tapa')
        ext_eye = (-28, -40, 26)
        tgt = (10.0, 7.75, 1.0)

        lid.isLightBulbOn = True
        cam = set_cam(app, ext_eye, tgt)
        snap(app, 'exterior_vista.png')
        render(app, design, 'render_exterior.png', cam)

        lid.isLightBulbOn = False
        cam = set_cam(app, (-6, -26, 45), tgt)
        snap(app, 'interior_vista.png')
        render(app, design, 'render_interior.png', cam)
        lid.isLightBulbOn = True

        # Explosionada: desplazamiento temporal en Z (se revierte al final)
        dz = {'Tapa': 16.0, 'ESP32_DevKit_30p': 12.0, 'Shield_ESP32': 10.0,
              'OLED_096': 10.0, 'Modulo_pH_PH4502C': 10.0, 'Modulo_Turbidez': 10.0,
              'Modulo_DS18B20': 10.0, 'PowerBank_2x18650': 10.0, 'Pilas_18650': 10.0,
              'Placa_acrilico': 7.0}
        saved = {}
        for n, d in dz.items():
            o = find_occ(root, n)
            if o:
                saved[n] = o.transform2.copy()
                m = o.transform2.copy()
                t = m.translation
                m.translation = adsk.core.Vector3D.create(t.x, t.y, t.z + d)
                o.transform2 = m
        cam = set_cam(app, ext_eye, (10.0, 7.75, 8))
        snap(app, 'explosionada_vista.png')
        render(app, design, 'render_explosionado.png', cam)
        for n, m in saved.items():
            find_occ(root, n).transform2 = m

        set_cam(app, ext_eye, tgt)
        txt = '\n'.join(LOG)
        with open(os.path.join(OUT, 'post_log.txt'), 'w', encoding='utf-8') as f:
            f.write(txt)
        ui.messageBox('Post-proceso listo:\n\n' + txt)
    except Exception:
        if ui:
            ui.messageBox('Error:\n{}'.format(traceback.format_exc()))
