# OpenGL

A repo of OpenGL related Projects

## Installs

### Install GLFW

```
sudo apt install libglfw3-dev

```

### Download ImGui

```
git clone https://github.com/ocornut/imgui.git

```

### Install gl3w

```
git clone https://github.com/skaslev/gl3w.git
cd gl3w
python3 gl3w_gen.py
```

This will generate:

```
gl3w.h and gl3w.c in gl3w/
```

Copy them into your project:

```
Move gl3w.h → include/GL/
Move gl3w.c → src/
```
