import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  server: {
    port: 5173,
    proxy: {
      // Proxy WS path (ws://localhost:22006/cs2_webradar)
      '/cs2_webradar': {
        target: 'ws://localhost:22006',
        ws: true,
        changeOrigin: true,
      },
     
      '/update': {
        target: 'http://localhost:22006',
        changeOrigin: true,
      }
    }
  }
})
