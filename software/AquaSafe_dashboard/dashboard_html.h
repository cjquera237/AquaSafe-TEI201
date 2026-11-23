// Pagina del dashboard, generada desde dashboard.html (editar ese archivo y regenerar).
#pragma once
#include <Arduino.h>

const char DASHBOARD_HTML[] PROGMEM = R"AQUASAFE(<!doctype html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>AquaSafe</title>
<style>
:root{--ink:#1E2B2F;--paper:#F1EEE6;--card:#FAF8F3;--line:#D9D3C5;--ochre:#C96A1B;--rust:#9A4A10;--glacier:#3E6E7E;--body:#3A474B;--ok:#2F6B4F;--bad:#A23B1E;--warn:#8A6A12}
*{box-sizing:border-box}
body{margin:0;background:var(--paper);color:var(--ink);font-family:system-ui,-apple-system,"Segoe UI",Roboto,Arial,sans-serif}
header{background:var(--ink);color:var(--paper);padding:14px 16px;display:flex;justify-content:space-between;align-items:center;gap:12px;flex-wrap:wrap}
header h1{margin:0;font-size:22px;letter-spacing:2px;text-transform:uppercase}
#conn{font-size:14px;color:#C9CFC9}
#conn.off{color:#F2A36B;font-weight:700}
main{max-width:1100px;margin:0 auto;padding:16px;display:flex;flex-direction:column;gap:16px}
#banner{padding:18px 20px;color:#FAF8F3;background:var(--glacier)}
#banner.ok{background:var(--ok)}#banner.bad{background:var(--bad)}
#banner b{display:block;font-size:26px;text-transform:uppercase;letter-spacing:1px}
#banner span{font-size:15px;opacity:.92}
.tiles{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:12px}
.tile{background:var(--card);border-top:6px solid var(--glacier);padding:14px 16px}
.tile.ok{border-top-color:var(--ok)}.tile.bad{border-top-color:var(--bad)}
.tile h2{margin:0;font-size:14px;text-transform:uppercase;letter-spacing:1px;color:var(--body)}
.val{font-size:38px;font-weight:700;margin:6px 0 2px}
.val small{font-size:16px;font-weight:600;color:var(--body)}
.ref{font-size:13px;color:var(--body)}
.tag{font-size:13px;font-weight:700}
.tile.ok .tag{color:var(--ok)}.tile.bad .tag{color:var(--bad)}
.bar{display:flex;gap:8px;flex-wrap:wrap;align-items:center}
.bar button,.bar a{font:inherit;font-size:14px;padding:8px 12px;border:2px solid var(--ink);background:var(--card);color:var(--ink);cursor:pointer;text-decoration:none}
.bar button.on{background:var(--ink);color:var(--paper)}
.bar .sp{flex:1}
.stats{display:grid;grid-template-columns:repeat(auto-fit,minmax(160px,1fr));gap:12px}
.stat{background:var(--card);padding:12px 14px}
.stat b{display:block;font-size:28px;color:var(--rust)}
.stat span{font-size:13px;color:var(--body)}
.charts{display:grid;grid-template-columns:repeat(auto-fit,minmax(320px,1fr));gap:12px}
.chart{background:var(--card);padding:12px 14px}
.chart h3{margin:0 0 4px;font-size:15px}
.chart p{margin:0 0 6px;font-size:13px;color:var(--body)}
canvas{width:100%;height:180px;display:block}
footer{font-size:12px;color:var(--body);text-align:center;padding:8px 16px 24px}
</style>
</head>
<body>
<header><h1>AquaSafe</h1><div id="conn">Conectando con el equipo…</div></header>
<main>
<section id="banner"><b id="bTitle">Esperando lecturas</b><span id="bSub">Sumerge las sondas en la muestra.</span></section>
<section class="tiles">
<div class="tile" id="tPh"><h2>pH</h2><div class="val" id="vPh">--</div><div class="ref">Referencia OMS: 6,5 – 8,5</div><div class="tag" id="gPh"></div></div>
<div class="tile" id="tTurb"><h2>Turbidez</h2><div class="val" id="vTurb">--</div><div class="ref">Meta OMS: menos de 1 UNT</div><div class="tag" id="gTurb"></div></div>
<div class="tile" id="tTds"><h2>Sólidos disueltos (TDS)</h2><div class="val" id="vTds">--</div><div class="ref">Referencia OMS: menos de 600 mg/L</div><div class="tag" id="gTds"></div></div>
<div class="tile" id="tTemp"><h2>Temperatura</h2><div class="val" id="vTemp">--</div><div class="ref">Compensa la lectura de TDS</div><div class="tag" id="gTemp"></div></div>
</section>
<section class="bar">
<span>Histórico:</span>
<button data-n="90">15 min</button><button data-n="360" class="on">1 hora</button><button data-n="0">Todo</button>
<span class="sp"></span>
<a href="/datos.csv" download="aquasafe_datos.csv">Descargar CSV</a>
<button id="clear">Borrar registro</button>
</section>
<section class="stats">
<div class="stat"><b id="sRows">0</b><span>mediciones en el periodo</span></div>
<div class="stat"><b id="sAlerts">0</b><span>alertas (muestra no potable)</span></div>
<div class="stat"><b id="sPct">0 %</b><span>del tiempo fuera de referencia</span></div>
<div class="stat"><b id="sLast">--</b><span>última medición guardada</span></div>
</section>
<section class="charts">
<div class="chart"><h3>pH</h3><p id="cPh">mín -- · prom -- · máx --</p><canvas id="chPh"></canvas></div>
<div class="chart"><h3>Turbidez</h3><p id="cTurb">mín -- · prom -- · máx --</p><canvas id="chTurb"></canvas></div>
<div class="chart"><h3>TDS</h3><p id="cTds">mín -- · prom -- · máx --</p><canvas id="chTds"></canvas></div>
<div class="chart"><h3>Temperatura</h3><p id="cTemp">mín -- · prom -- · máx --</p><canvas id="chTemp"></canvas></div>
</section>
</main>
<footer>Una medición guardada cada 10 s en la memoria del ESP32 (datos.csv) · pantalla actualizada cada 2 s · el equipo no detecta Giardia: filtrar o hervir siempre.</footer>
<script>
const $=id=>document.getElementById(id);
let rows=[],lastRows=-1,range=360,timeSent=false;
const num=v=>(v===null||v===undefined||v===''||isNaN(+v))?null:+v;
const fmt=(v,d)=>v===null?'--':v.toFixed(d).replace('.',',');
const POT='REF. POTABLE';

function parseCsv(t){
  const out=[];const lines=t.trim().split('\n');
  for(let i=1;i<lines.length;i++){const c=lines[i].split(',');if(c.length<10)continue;
    out.push({fecha:c[0],up:num(c[1]),temp:num(c[2]),ph:num(c[3]),phV:num(c[4]),tds:num(c[5]),turb:num(c[6]),turbV:num(c[7]),oms:c[8],muestra:c[9].trim()});}
  return out;
}
function fromJson(j){return{fecha:j.log.fecha,up:j.log.up,temp:num(j.log.temp),ph:num(j.log.ph),phV:num(j.log.ph_v),tds:num(j.log.tds),turb:num(j.log.turb),turbV:num(j.log.turb_v),oms:j.log.estado,muestra:j.log.muestra};}

async function loadHistory(){
  try{const r=await fetch('/datos.csv',{cache:'no-store'});rows=r.ok?parseCsv(await r.text()):[];}catch(e){}
  draw();
}
function setTile(id,cls,tag){const t=$(id);t.className='tile'+(cls?' '+cls:'');$(id.replace('t','g')).textContent=tag;}

function showNow(j){
  const ph=num(j.ph),phV=num(j.ph_v),turb=num(j.turb),turbV=num(j.turb_v),tds=num(j.tds),temp=num(j.temp);
  $('vPh').innerHTML=ph!==null?fmt(ph,2):(phV!==null?fmt(phV,3)+' <small>V sin calibrar</small>':'--');
  $('vTurb').innerHTML=turb!==null?fmt(turb,2)+' <small>UNT</small>':(turbV!==null?fmt(turbV,3)+' <small>V sin calibrar</small>':'--');
  $('vTds').innerHTML=tds!==null?fmt(tds,0)+' <small>mg/L</small>':'--';
  $('vTemp').innerHTML=temp!==null?fmt(temp,1)+' <small>°C</small>':'--';
  if(ph!==null){const ok=ph>=6.5&&ph<=8.5;setTile('tPh',ok?'ok':'bad',ok?'Dentro de referencia':'Fuera de referencia');}else setTile('tPh','',ph===null&&phV!==null?'Calibrar con «ph 1» y «ph 2»':'');
  if(turb!==null){const ok=turb<1;setTile('tTurb',ok?'ok':'bad',ok?'Dentro de la meta':'Sobre la meta');}else setTile('tTurb','',turbV!==null?'Calibrar con «turb 1» y «turb 2»':'');
  if(tds!==null){const ok=tds<600;setTile('tTds',ok?'ok':'bad',ok?'Dentro de referencia':'Sobre la referencia');}else setTile('tTds','','');
  setTile('tTemp','',temp===null?'Sin lectura':'');
  const m=(j.muestra||'').replace('\n',' ');const b=$('banner');
  if(m===POT){b.className='ok';$('bTitle').textContent='Agua dentro de referencia';}
  else if(m.indexOf('TURBIA')>=0||m.indexOf('NO SE RECOMIENDA')>=0){b.className='bad';$('bTitle').textContent='No se recomienda consumir';}
  else{b.className='';$('bTitle').textContent=m||'Esperando lecturas';}
  $('bSub').textContent='Muestra: '+(m||'--')+' · Referencias OMS: '+(j.estado||'--')+' · Aunque salga dentro de referencia, filtrar o hervir.';
}

async function poll(){
  try{
    const r=await fetch('/api/now',{cache:'no-store'});const j=await r.json();
    $('conn').textContent='Conectado · '+(j.fecha||('encendido hace '+Math.round(j.uptime/60)+' min'));$('conn').className='';
    if(!timeSent&&!j.reloj){timeSent=true;fetch('/api/time?epoch='+Math.floor(Date.now()/1000)+'&tz='+new Date().getTimezoneOffset());}
    showNow(j);
    if(lastRows>=0&&j.rows===lastRows+1&&j.log){rows.push(fromJson(j));draw();}
    else if(j.rows!==lastRows){await loadHistory();}
    lastRows=j.rows;
  }catch(e){$('conn').textContent='Sin conexión con el equipo';$('conn').className='off';}
}

function series(list,key,alt){return list.map(r=>r[key]!==null?r[key]:(alt?r[alt]:null));}
function stats(v){const a=v.filter(x=>x!==null);if(!a.length)return null;let s=0,mi=a[0],ma=a[0];for(const x of a){s+=x;if(x<mi)mi=x;if(x>ma)ma=x;}return{mi,ma,av:s/a.length};}
function unitOf(list,key){return list.some(r=>r[key]!==null);}

function chart(id,vals,refs,dec){
  const c=$(id),dpr=window.devicePixelRatio||1,w=c.clientWidth,h=c.clientHeight;
  c.width=w*dpr;c.height=h*dpr;const g=c.getContext('2d');g.scale(dpr,dpr);g.clearRect(0,0,w,h);
  const a=vals.filter(x=>x!==null);const L=44,R=8,T=8,B=18;
  g.font='12px system-ui,Arial';g.fillStyle='#3A474B';
  if(a.length<2){g.fillText('Aún no hay suficientes datos',L,h/2);return;}
  let mi=Math.min(...a),ma=Math.max(...a);for(const r of refs){mi=Math.min(mi,r);ma=Math.max(ma,r);}
  if(ma-mi<1e-6){ma+=1;mi-=1;}const pad=(ma-mi)*0.1;mi-=pad;ma+=pad;
  const X=i=>L+(w-L-R)*i/(vals.length-1),Y=v=>T+(h-T-B)*(1-(v-mi)/(ma-mi));
  g.strokeStyle='#D9D3C5';g.lineWidth=1;
  for(let k=0;k<=3;k++){const v=mi+(ma-mi)*k/3,y=Y(v);g.beginPath();g.moveTo(L,y);g.lineTo(w-R,y);g.stroke();g.fillText(v.toFixed(dec).replace('.',','),2,y+4);}
  g.setLineDash([6,4]);g.strokeStyle='#9A4A10';
  for(const r of refs){const y=Y(r);g.beginPath();g.moveTo(L,y);g.lineTo(w-R,y);g.stroke();}
  g.setLineDash([]);g.strokeStyle='#3E6E7E';g.lineWidth=2;g.beginPath();let pen=false;
  vals.forEach((v,i)=>{if(v===null){pen=false;return;}const x=X(i),y=Y(v);pen?g.lineTo(x,y):g.moveTo(x,y);pen=true;});g.stroke();
  g.fillStyle='#3A474B';g.fillText(vals.length+' puntos',L,h-3);
}
function fillStats(id,v,dec,u){const s=stats(v);$(id).textContent=s?('mín '+fmt(s.mi,dec)+' · prom '+fmt(s.av,dec)+' · máx '+fmt(s.ma,dec)+(u?' '+u:'')):'mín -- · prom -- · máx --';}

function draw(){
  const list=range?rows.slice(-range):rows;
  const phCal=unitOf(list,'ph'),turbCal=unitOf(list,'turb');
  const ph=phCal?series(list,'ph'):series(list,'phV'),turb=turbCal?series(list,'turb'):series(list,'turbV');
  const tds=series(list,'tds'),temp=series(list,'temp');
  chart('chPh',ph,phCal?[6.5,8.5]:[],phCal?1:2);fillStats('cPh',ph,2,phCal?'':'V');
  chart('chTurb',turb,turbCal?[1]:[],2);fillStats('cTurb',turb,2,turbCal?'UNT':'V');
  chart('chTds',tds,[600],0);fillStats('cTds',tds,0,'mg/L');
  chart('chTemp',temp,[],1);fillStats('cTemp',temp,1,'°C');
  const al=list.filter(r=>r.muestra&&r.muestra!==POT&&r.muestra.indexOf('SIN LECTURA')<0).length;
  $('sRows').textContent=list.length;$('sAlerts').textContent=al;
  $('sPct').textContent=(list.length?Math.round(100*al/list.length):0)+' %';
  const last=list[list.length-1];$('sLast').textContent=last?(last.fecha?last.fecha.slice(11,19):Math.round(last.up)+' s'):'--';
}

document.querySelectorAll('[data-n]').forEach(b=>b.onclick=()=>{document.querySelectorAll('[data-n]').forEach(x=>x.classList.remove('on'));b.classList.add('on');range=+b.dataset.n;draw();});
$('clear').onclick=async()=>{if(!confirm('¿Borrar todas las mediciones guardadas en el equipo?'))return;await fetch('/api/borrar',{method:'POST'});rows=[];lastRows=-1;draw();};
window.addEventListener('resize',draw);
loadHistory();poll();setInterval(poll,2000);
</script>
</body>
</html>
)AQUASAFE";
