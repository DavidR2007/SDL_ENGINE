import tkinter as tk
import sys

def main():
    try:
        root = tk.Tk()
        root.withdraw() # ocultar ventana de tkinter
        img = tk.PhotoImage(file='resources/asteroids_spritesheet.png')
        w = img.width()
        h = img.height()
        print(f"Dimensiones de la hoja de sprites: {w}x{h}")

        # Escanear pixeles no transparentes
        non_transparent = []
        for y in range(h):
            for x in range(w):
                # PhotoImage tiene transparency_get en algunas versiones de Tk, o podemos usar get()
                # y chequear si es distinto de negro/blanco o transparent.
                # get(x, y) devuelve una string con formato "r g b" o tupla
                try:
                    is_transparent = img.transparency_get(x, y)
                except Exception:
                    # Fallback si no está disponible transparency_get
                    pixel = img.get(x, y)
                    # En algunos sistemas devuelve tuple, en otros string
                    if isinstance(pixel, str):
                        parts = [int(p) for p in pixel.split()]
                    else:
                        parts = pixel
                    is_transparent = len(parts) > 3 and parts[3] == 0
                
                if not is_transparent:
                    # En la hoja de asteroids, los sprites son blancos/grises y el fondo es transparente o negro.
                    # Comprobamos si tiene color (RGB no negro)
                    pixel = img.get(x, y)
                    if isinstance(pixel, str):
                        r, g, b = [int(p) for p in pixel.split()[:3]]
                    else:
                        r, g, b = pixel[:3]
                    
                    if r > 10 or g > 10 or b > 10: # no negro
                        non_transparent.append((x, y))

        print(f"Pixeles con color encontrados: {len(non_transparent)}")

        # Agrupar pixeles en componentes conectados (DFS/BFS) para encontrar los sprites independientes
        visited = set()
        components = []
        pixel_set = set(non_transparent)

        for px, py in non_transparent:
            if (px, py) in visited:
                continue
            
            # BFS para encontrar toda la figura conectada
            queue = [(px, py)]
            visited.add((px, py))
            comp = []
            
            while queue:
                cx, cy = queue.pop(0)
                comp.append((cx, cy))
                # Vecinos (8 direcciones)
                for dx in [-1, 0, 1]:
                    for dy in [-1, 0, 1]:
                        if dx == 0 and dy == 0:
                            continue
                        nx, ny = cx + dx, cy + dy
                        if (nx, ny) in pixel_set and (nx, ny) not in visited:
                            visited.add((nx, ny))
                            queue.append((nx, ny))
            
            components.append(comp)

        print(f"Componentes (sprites) independientes encontrados: {len(components)}")

        # Imprimir la caja delimitadora (bbox) de cada componente
        for idx, comp in enumerate(components):
            xs = [p[0] for p in comp]
            ys = [p[1] for p in comp]
            min_x, max_x = min(xs), max(xs)
            min_y, max_y = min(ys), max(ys)
            width = max_x - min_x + 1
            height = max_y - min_y + 1
            print(f"Sprite #{idx+1}: X={min_x}, Y={min_y}, W={width}, H={height}, Pixeles={len(comp)}")

    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
