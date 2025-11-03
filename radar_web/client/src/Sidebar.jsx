import React from "react"

export default function Sidebar({
  playerNames,
  highlightedPlayer, setHighlightedPlayer,
  showNames, setShowNames,
  showHealth, setShowHealth,
  showWeapon, setShowWeapon,
  showDirection, setShowDirection,
}) {
  return (
    <aside className="w-[280px] shrink-0 p-4">
<div className="rounded-2xl border border-white/10 bg-neutral-900/80 backdrop-blur-md shadow-2xl">

        <div className="px-4 pt-4 pb-2 border-b border-white/10">
          <h2 className="text-teal-300 text-sm font-semibold tracking-wide">Radar Settings</h2>
        </div>

        <div className="p-4 space-y-5">
          <div className="space-y-2">
            <label className="text-xs font-medium text-neutral-300">Highlight Player</label>
            <select
              value={highlightedPlayer || ""}
              onChange={(e) => setHighlightedPlayer(e.target.value)}
              className="w-full rounded-lg bg-neutral-900/70 border border-white/10 text-sm px-3 py-2 outline-none focus:ring-2 focus:ring-teal-400/40 focus:border-teal-400/40 transition"
            >
              <option value="">None</option>
              {playerNames.map(n => (
                <option key={n} value={n}>{n}</option>
              ))}
            </select>
          </div>

          <div className="space-y-3">
            <Toggle label="Show Names" checked={showNames} onChange={setShowNames}/>
            <Toggle label="Show Health" checked={showHealth} onChange={setShowHealth}/>
            <Toggle label="Show Weapon" checked={showWeapon} onChange={setShowWeapon}/>
            <Toggle label="Show View Direction" checked={showDirection} onChange={setShowDirection}/>
          </div>
        </div>
      </div>
    </aside>
  )
}

function Toggle({ label, checked, onChange }) {
  return (
    <label className="flex items-center justify-between gap-4 text-sm">
      <span className="text-neutral-200">{label}</span>
      {/* nice switch built with Tailwind; still a native checkbox for accessibility */}
      <span className="relative inline-flex items-center">
        <input
          type="checkbox"
          className="peer sr-only"
          checked={checked}
          onChange={(e) => onChange(e.target.checked)}
        />
        <span className="h-6 w-10 rounded-full bg-white/10 border border-white/10 peer-checked:bg-teal-500/70 transition-colors"></span>
        <span className="absolute left-0.5 top-0.5 h-5 w-5 rounded-full bg-neutral-200 shadow
                         peer-checked:translate-x-4 transform transition-transform"></span>
      </span>
    </label>
  )
}
