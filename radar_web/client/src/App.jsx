import React, { useState } from "react"
import Sidebar from "./Sidebar.jsx"
import RadarShell from "./RadarShell.jsx"

export default function App() {
  const [showNames, setShowNames] = useState(true)
  const [showHealth, setShowHealth] = useState(true)
  const [showWeapon, setShowWeapon] = useState(true)
  const [showDirection, setShowDirection] = useState(true)
  const [highlightedPlayer, setHighlightedPlayer] = useState("")
  const [playerNames, setPlayerNames] = useState([])

  return (
    <div className="min-h-screen bg-gradient-to-b from-neutral-950 via-neutral-900 to-neutral-950 text-neutral-100">
      {/* Top bar */}
      <header className="sticky top-0 z-10 backdrop-blur border-b border-white/10 bg-neutral-950/50">
        <div className="max-w-7xl mx-auto px-4 py-3 flex items-center gap-3">
          <div className="h-2.5 w-2.5 rounded-full bg-emerald-400/80 shadow-[0_0_12px_rgba(16,185,129,0.8)]"></div>
          <h1 className="text-base font-semibold tracking-tight">WebRadar</h1>
          <span className="ml-auto text-xs text-neutral-400">WebRadar</span>
        </div>
      </header>

      {/* Content */}
      <main className="max-w-7xl mx-auto flex">
        <Sidebar
          playerNames={playerNames}
          highlightedPlayer={highlightedPlayer}
          setHighlightedPlayer={setHighlightedPlayer}
          showNames={showNames} setShowNames={setShowNames}
          showHealth={showHealth} setShowHealth={setShowHealth}
          showWeapon={showWeapon} setShowWeapon={setShowWeapon}
          showDirection={showDirection} setShowDirection={setShowDirection}
        />
        <div className="flex-1 overflow-auto">
          <RadarShell
            showNames={showNames}
            showHealth={showHealth}
            showWeapon={showWeapon}
            showDirection={showDirection}
            highlightedPlayer={highlightedPlayer}
            onPlayerNamesChange={setPlayerNames}
          />
        </div>
      </main>
    </div>
  )
}
