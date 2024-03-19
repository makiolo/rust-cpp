Implementar calculadores de macrooption:

https://www.macroption.com/black-scholes-calculator/

# Instalar Tensorflow

# conda no me funciona, usar instaladores
conda install -c conda-forge cudatoolkit=11.8 cudnn=8.6.0

pip install tensorflow==2.10

# Instalar Jupyter

pip install ipykernel
python -m ipykernel install --user
pip install notebook

# Generate ".h"

- cbindgen --config cbindgen.toml --crate red_pandas --output libs/red_pandas.h


# Configurar preset y compilar

vcpkg install --triplet x64-windows
cmake -B build -S . --preset=win-x64-release
cmake --build build --config Release

# Probar Bazel

https://medium.com/@Vertexwahn/bazel-as-an-alternative-to-cmake-fb7c86d95b48
