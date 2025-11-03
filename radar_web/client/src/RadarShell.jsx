import React from "react"
import RadarCanvas from "./RadarCanvas.jsx"

export default function RadarShell(props) {
  return (
    <div className="p-4 w-full">
      <div className="rounded-2xl border border-white/10 bg-white/5 backdrop-blur-sm shadow-2xl">
        <div className="flex items-center justify-between px-4 py-3 border-b border-white/10">
          <h3 className="text-sm font-semibold text-neutral-200">Live Radar</h3>
          <div className="flex items-center gap-2">
          <span className="text-sm text-neutral-400">🟢 Live Users:</span>
          <span id="liveUserCount" className="font-bold text-green-400">0</span>
          </div>
        </div>

        <div className="p-3">
          <div className="rounded-xl border border-white/10 overflow-hidden">
            {/*  existing canvas logic lives inside */}
            <RadarCanvas {...props} />
          </div>
        </div>
      </div>
    </div>
  )
}
