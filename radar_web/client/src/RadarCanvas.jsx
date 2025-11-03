import React, { useEffect, useRef, useState } from 'react';

// ---------- helpers ----------
function normalizeMapKey(raw) {
  if (!raw) return null;
  let s = String(raw).toLowerCase().replace(/\\/g, '/'); // windows -> posix
  s = s.split('/').pop();                // "maps/de_dust2.vpk" -> "de_dust2.vpk"
  s = s.replace(/\.(bsp|vpk)$/i, '');    // "de_dust2.vpk" -> "de_dust2"
  return s;
}

const WS_URL =
  import.meta.env.DEV
    ? 'ws://localhost:22006/cs2_webradar'
    : (location.protocol === 'https:' ? 'wss://' : 'ws://') + location.host + '/cs2_webradar';

export default function RadarCanvas({
  showNames,
  showHealth,
  showWeapon,
  showDirection,
  highlightedPlayer,
  onPlayerNamesChange,
}) {
  const canvasRef = useRef(null);
  const ctxRef = useRef(null);

  const [mapData, setMapData] = useState(null);
  const [players, setPlayers] = useState([]);
  const [bombs, setBombs] = useState([]);
  const [mapReady, setMapReady] = useState(false);

  // images
  const backgroundImgRef = useRef(new Image());
  const radarImgRef = useRef(new Image());
  const bombImgRef = useRef(new Image());

  // map cache + cooldown (TOP-LEVEL REFS, not inside effects)
  const lastMapNameRef = useRef(null);
  const lastMapChangeTimeRef = useRef(0);
  const MAP_UPDATE_COOLDOWN = 5000; // 5s

  // ---------- map loading ----------
  const loadMap = (mapName) => {
    if (!mapName) return;
    console.log('[MAP] loading', mapName);

    setMapReady(false);

    const bg = backgroundImgRef.current;
    const rad = radarImgRef.current;

    const bgSrc = `/maps/${mapName}/background.png`;
    const radarSrc = `/maps/${mapName}/radar.png`;
    const jsonUrl = `/maps/${mapName}/data.json`;

    bg.onload = () => console.log('[MAP] background onload', bg.naturalWidth, bg.naturalHeight);
    bg.onerror = () => console.error('[MAP] background failed:', bgSrc);

    rad.onload = () => {
      console.log('[MAP] radar onload', rad.naturalWidth, rad.naturalHeight);
      const canvas = canvasRef.current;
      if (canvas) {
        
        const w = rad.naturalWidth || rad.width || canvas.width || 1024;
        const h = rad.naturalHeight || rad.height || canvas.height || 1024;
        canvas.width = w;
        canvas.height = h;
        console.log('[MAP] canvas size set', w, h);
      }
      setMapReady(true);
    };
    rad.onerror = () => console.error('[MAP] radar failed:', radarSrc);

    // set sources AFTER handlers
    bg.src = bgSrc;
    rad.src = radarSrc;

    fetch(jsonUrl)
      .then((r) => {
        if (!r.ok) throw new Error(`${r.status} ${r.statusText}`);
        return r.json();
      })
      .then((data) => {
        console.log('[MAP] data.json OK for', mapName, data);
        setMapData({ ...data, mapName });
      })
      .catch((err) => console.error('[MAP] data.json failed:', jsonUrl, err));
  };

  // ---------- helpers ----------
  const normalizePosition = (pos) => {
    if (!mapData || !mapData.scale || mapData.x === undefined || mapData.y === undefined) {
      return { x: 0, y: 0 };
    }
    const x = (pos.x - mapData.x) / mapData.scale;
    const y = (mapData.y - pos.y) / mapData.scale;
    return { x, y };
  };

  const getHealthColor = (health) => {
    if (health >= 100) return '#00ff00';
    if (health >= 75) return '#ffff00';
    if (health >= 50) return '#ff9900';
    return '#ff0000';
  };

  // ---------- draw loop ----------
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;


    //if images havent loaded yet
    if (!canvas.width) canvas.width = 1024;
    if (!canvas.height) canvas.height = 1024;

    const ctx = canvas.getContext('2d');
    ctxRef.current = ctx;
    ctx.imageSmoothingEnabled = true;

    let rafId;
    const draw = () => {
      const ctx = ctxRef.current;
      if (!ctx) return;

      const bg = backgroundImgRef.current;
      const rad = radarImgRef.current;
      const bombImg = bombImgRef.current;

      ctx.clearRect(0, 0, canvas.width, canvas.height);

      if (mapReady) {
        if (bg && bg.complete) ctx.drawImage(bg, 0, 0, canvas.width, canvas.height);
        if (rad && rad.complete) ctx.drawImage(rad, 0, 0, canvas.width, canvas.height);
      }

      // bomb status
      if (Array.isArray(bombs) && bombs.length > 0) {
        const bomb = bombs[0];
        const bombImg = bombImgRef.current;
      
        // --- Top-center status text ---
        let statusText = '';
        let textColor = '#ffffff';
      
        if (bomb.m_time > 0) {
          statusText = `Bomb planted - ${bomb.m_time.toFixed(1)}s to explode`;
      
         
          if (bomb.m_time < 5) textColor = '#ff0000';
          else if (bomb.m_time < 10) textColor = '#f5ed07';
          else textColor = '#00ff00';
      
      
          if (bomb.m_defusing && bomb.m_defuse_time > 0) {
            statusText += ` | Defusing - ${bomb.m_defuse_time.toFixed(1)}s left`;
            
            textColor = '#00ff00';
          }
        } else if (bomb.m_defusing && bomb.m_defuse_time > 0) {
         
          statusText = `Defusing - ${bomb.m_defuse_time.toFixed(1)}s left`;
          textColor = '#00ff00';
        }
      
        if (statusText) {
          ctx.fillStyle = textColor;
          ctx.font = 'bold 16px Verdana';
          ctx.textAlign = 'center';
          ctx.fillText(statusText, canvas.width / 2, 28);
          ctx.textAlign = 'start';
        }
      
        // --- Draw bomb icon on map ---
        const pos = normalizePosition(bomb.m_position || { x: 0, y: 0 });
        const x = pos.x;
        const y = pos.y;
        const iconSize = 24;
      
        if (bombImg && bombImg.complete) {
          ctx.drawImage(bombImg, x - iconSize / 2, y - iconSize / 2, iconSize, iconSize);
        }
      
        // draw a glow
        ctx.beginPath();
        ctx.arc(x, y, iconSize / 1.5, 0, 2 * Math.PI);
        ctx.strokeStyle = textColor;
        ctx.lineWidth = 2;
        ctx.stroke();
      }

      // players
      players.forEach((p) => {
        const pos = normalizePosition(p.m_position || { x: 0, y: 0 });
        const x = pos.x;
        const y = pos.y;

        const color =
          highlightedPlayer && p.m_name === highlightedPlayer
            ? 'yellow'
            : p.m_team === 2
            ? '#ff0000'
            : '#00ffff';

        ctx.beginPath();
        ctx.arc(x, y, 6, 0, 2 * Math.PI);
        ctx.fillStyle = color;
        ctx.fill();

        if (showDirection && p.m_eye_angle && typeof p.m_eye_angle.y === 'number') {
          const yawDeg = p.m_eye_angle.y;
          const angleRad = (yawDeg * Math.PI) / 180;
        
          const length = 20;
          const dx = Math.cos(angleRad) * length;
          const dy = -Math.sin(angleRad) * length; //invert Y for canvas
        
          ctx.strokeStyle = color;
          ctx.beginPath();
          ctx.moveTo(x, y);
          ctx.lineTo(x + dx, y + dy);
          ctx.stroke();
        }

        ctx.fillStyle = '#fff';
        ctx.font = '12px Verdana';

        if (showNames || showHealth) {
          const baseY = y - 12;
          if (showNames) {
            ctx.fillStyle = '#ffffff';
            ctx.fillText(p.m_name || '', x + 8, baseY);
          }
          if (showHealth) {
            ctx.fillStyle = getHealthColor(p.m_health ?? 0);
            const nameWidth = ctx.measureText(p.m_name || '').width;
            ctx.fillText(`(${p.m_health ?? 0})`, x + 8 + nameWidth + 4, baseY);
          }
        }

        if (showWeapon && p.m_weapon && p.m_weapon.length > 0) {
          ctx.font = '11px Verdana';
          ctx.fillStyle = '#cccccc';
          ctx.fillText(p.m_weapon, x + 8, y + 4);
        }
      });

      rafId = requestAnimationFrame(draw);
    };

    bombImgRef.current.src = '/bomb.png';

    rafId = requestAnimationFrame(draw);
    return () => cancelAnimationFrame(rafId);
  }, [players, bombs, showNames, showHealth, showWeapon, showDirection, highlightedPlayer, mapReady, mapData]);

  // WebSocket
  useEffect(() => {
    const ws = new WebSocket(WS_URL);

    ws.onopen = () => console.log('[WS] connected', WS_URL);
    ws.onclose = () => console.log('[WS] disconnected');
    ws.onerror = (e) => console.log('[WS] error', e);

    ws.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);

        const incomingMapRaw =
          data.m_map || data.map || data.map_name || data.level || data.levelName || null;
        const incomingMap = normalizeMapKey(incomingMapRaw);
        const now = Date.now();

        const newPlayers = Array.isArray(data.m_players) ? data.m_players : [];
        const newBombs = Array.isArray(data.m_bombs) ? data.m_bombs : [];
        setPlayers(newPlayers);
        setBombs(newBombs);

        const names = [...new Set(newPlayers.map((p) => p.m_name).filter(Boolean))];
        onPlayerNamesChange(names);

        if (incomingMap) {
          const mapChanged = incomingMap !== lastMapNameRef.current;
          const timePassed = now - lastMapChangeTimeRef.current > MAP_UPDATE_COOLDOWN;

          if (mapChanged || timePassed) {
            console.log(`[MAP] checking map: current=${lastMapNameRef.current}, incoming=${incomingMap}`);
            lastMapNameRef.current = incomingMap;
            lastMapChangeTimeRef.current = now;

            if (!mapData || mapData.mapName !== incomingMap) {
              console.log('[MAP] switching map to', incomingMap);
              loadMap(incomingMap);
            }
          }
        } else if (!lastMapNameRef.current && !mapData) {
          console.warn('[MAP] no map provided, loading default');
          lastMapNameRef.current = 'de_inferno';
          loadMap('de_inferno');
        }
      } catch (e) {
        console.error('[WS] invalid JSON:', e);
      }
    };

    return () => ws.close();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  // Give canvas a default size so it's never 0×0 before images load
  return <canvas id="radar" ref={canvasRef} width={1024} height={1024} className="block" />;
}
