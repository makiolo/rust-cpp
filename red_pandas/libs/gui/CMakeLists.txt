


# 1 llega un mensaje JSON/YAML
# 2 devuelve 200 si algun job acepta su esquema y name de job valido
# 3 se mete en la cola de requests
# 4 si hay ejecutores libres, se mete en la cola de cada job
# 5 el job lee de su cola, ejecuta el job y devuelve SUCCESS, UNSTABLE, FAILED, ABORTED, ERROR.
# 6. Se almacena el resultado en una tabla de la base de datos con el status y el tiempo de ejecución.



# Comunicacion exterior: REST, RPC, WebSockets ...
# Llega una request (con el nombre del job y los parametros)
# Se mete en la cola
# Se va despachando si hay ejecutores libres
# El job es un behaviour tree parametrizado
# Desde la ejecución puede:
# - Escribir Log o Backtraces
# - Enviar mensajes a otros jobs, aunque idealmente un job debe ser independiente
# - Enviar mensajes a otros servicios (zeromq, http, rpc, websockets ...)
# - Leer y escribir en una base de datos
# - Leer y escribir en un archivo
# - Leer y escribir en una cola de mensajes
# - Leer y escribir en una cola de eventos
# - Calcular querys de la base de datos y almacenarlo en una tabla
find_package(fmt CONFIG REQUIRED)
# Download
find_package(CURL REQUIRED)
# User interfacde
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
# Send and receive data
find_package(ZeroMQ CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
# Save and load data
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(SqliteOrm CONFIG REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)
# Algorithms
find_package(ParallelSTL CONFIG REQUIRED)
# Calculate on data
find_package(DataFrame CONFIG REQUIRED)
# Ejecutar Jobs (Un scheduler de corutinas)
find_package(behaviortree_cpp CONFIG REQUIRED)
# Organizacion de Objetos Reutilizables
find_package(EnTT CONFIG REQUIRED)
# Optimizar memoria
find_package(mimalloc CONFIG REQUIRED)
# include(FetchContent)
# FetchContent_Declare(
#         libcoro
#         GIT_REPOSITORY https://github.com/jbaldwin/libcoro.git
#         GIT_TAG        v0.11.1
# )
# FetchContent_MakeAvailable(libcoro)


# GLAD
add_subdirectory(depends/implot_demos/3rdparty/glad)

# NFD
add_subdirectory(depends/implot_demos/3rdparty/nativefiledialog-extended)

# KISS FFT
# set(KISS_SRC    "depends/implot_demos/3rdparty/kissfft/kiss_fft.h"
#                 "depends/implot_demos/3rdparty/kissfft/kiss_fftr.h"
#                 "depends/implot_demos/3rdparty/kissfft/kiss_fft.c"
#                 "depends/implot_demos/3rdparty/kissfft/kiss_fftr.c")

# EVERYTHING ELSE
# include_directories("3rdparty" "depends/implot_demos/3rdparty/kissfft")

###############################################################################
# IMGUI
###############################################################################

set(IMGUI_HEADERS
        depends/imgui/imconfig.h
        depends/imgui/imgui_internal.h
        depends/imgui/imgui.h
        depends/imgui/imstb_rectpack.h
        depends/imgui/imstb_textedit.h
        depends/imgui/imstb_truetype.h
        depends/imgui/backends/imgui_impl_glfw.h
        depends/imgui/backends/imgui_impl_opengl3.h
        depends/imgui/backends/imgui_impl_opengl3_loader.h
        depends/imgui/misc/cpp/imgui_stdlib.h
)

set(IMGUI_SRC
        depends/imgui/imgui_demo.cpp
        depends/imgui/imgui_draw.cpp
        depends/imgui/imgui_tables.cpp
        depends/imgui/imgui_widgets.cpp
        depends/imgui/imgui.cpp
        depends/imgui/backends/imgui_impl_glfw.cpp
        depends/imgui/backends/imgui_impl_opengl3.cpp
        depends/imgui/misc/cpp/imgui_stdlib.cpp
)

add_library(imgui ${IMGUI_HEADERS} ${IMGUI_SRC})
if(MSVC)
        target_compile_options(imgui PRIVATE /W4 /WX /arch:AVX2 /fp:fast)
endif()

# imm32 is needed for Multimedia Input
target_link_libraries(imgui PUBLIC glfw glad OpenGL::GL imm32)
target_compile_definitions(imgui PRIVATE IMGUI_DLL_EXPORT)

include_directories(
        depends/imgui/
        depends/imgui/examples
        depends/imgui/examples/libs/gl3w
        depends/imgui/backends
        depends/imgui/misc/cpp
)

# imnodes
# add_library(imnodes     depends/implot_demos/3rdparty/imnodes/imnodes.h
#                         depends/implot_demos/3rdparty/imnodes/imnodes_internal.h
#                         depends/implot_demos/3rdparty/imnodes/imnodes.cpp)
# target_link_libraries(imnodes imgui)
# include_directories(depends/implot_demos/3rdparty/imnodes)

###############################################################################
# IMPLOT
###############################################################################

set(IMPLOT_HEADERS depends/implot/implot.h
                   depends/implot/implot_internal.h)

set(IMPLOT_SRC     depends/implot/implot.cpp
                   depends/implot/implot_items.cpp
                   depends/implot/implot_demo.cpp)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/depends/implot/backends")
        list(APPEND IMPLOT_HEADERS depends/implot/backends/implot_backend.h
                                   depends/implot/backends/implot_impl_opengl3.h)
        list(APPEND IMPLOT_SRC     depends/implot/backends/implot_backend.h
                                   depends/implot/backends/implot_impl_opengl3.cpp)
endif()

add_library(implot STATIC ${IMPLOT_HEADERS} ${IMPLOT_SRC})
target_link_libraries(implot PUBLIC imgui)
target_compile_definitions(implot PUBLIC
        IMPLOT_DEBUG IMPLOT_DLL_EXPORT
        IMPLOT_BACKEND_ENABLE_OPENGL3
        IMGUI_IMPL_OPENGL_LOADER_GLAD)
set_property(TARGET implot PROPERTY CXX_STANDARD 20)

if(MSVC)
        target_compile_options(implot PRIVATE /W4 /WX /arch:AVX2 /fp:fast /permissive-)
else()
        target_compile_options(implot PRIVATE -Wall -Wextra -pedantic -Werror -mavx2 -Ofast)
endif()

include_directories(depends/implot/)


set(PRIVATE_sources
        libs/gui/src/gui.cpp
        # libs/gui/src/producer.cpp
        depends/implot_demos/common/App.cpp
        depends/implot_demos/common/Native.cpp
        depends/implot_demos/common/Fonts/FontAwesome5.cpp
        depends/implot_demos/common/Fonts/RobotoRegular.cpp
        depends/implot_demos/common/Fonts/RobotoBold.cpp
        depends/implot_demos/common/Fonts/RobotoItalic.cpp
        depends/implot_demos/common/Fonts/RobotoMonoRegular.cpp
        depends/implot_demos/common/Fonts/RobotoMonoBold.cpp
        depends/implot_demos/common/Fonts/RobotoMonoItalic.cpp
        )
set(PUBLIC_sources
        libs/gui/include/gui.h
        # libs/gui/include/producer.h
        depends/implot_demos/common/App.h
        depends/implot_demos/common/Shader.h
        depends/implot_demos/common/Native.h
        depends/implot_demos/common/Image.h
        depends/implot_demos/common/Fonts/Fonts.h
        )
set(PUBLIC_include_dirs
        libs/gui/include/
        depends/pybind11/include/
        )
set(PRIVATE_include_dirs
        depends/implot_demos/common/
        depends/implot_demos/3rdparty/
)
set(PRIVATE_targets
        fmt
        CURL::libcurl
        red_pandas_static
        implot
        nfd
        )
set(PUBLIC_targets

        )
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_executable(gui)
else()
        add_executable(gui WIN32)
endif()
target_sources(gui PRIVATE ${PRIVATE_sources} PUBLIC ${PUBLIC_sources})
target_include_directories(gui PUBLIC ${PUBLIC_include_dirs})
target_include_directories(gui PRIVATE ${PRIVATE_include_dirs})
target_compile_features(gui PUBLIC cxx_std_20)
set_target_properties(gui PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(gui PRIVATE ${PRIVATE_targets})
target_link_libraries(gui PUBLIC ${PUBLIC_targets})
target_link_libraries(gui PRIVATE libzmq libzmq-static)
target_link_libraries(gui PRIVATE cppzmq cppzmq-static)
target_link_libraries(gui PRIVATE unofficial::sqlite3::sqlite3)
target_link_libraries(gui PRIVATE SQLiteCpp)
target_link_libraries(gui PRIVATE sqlite_orm::sqlite_orm)
target_link_libraries(gui PRIVATE pstl::ParallelSTL)
target_link_libraries(gui PRIVATE DataFrame::DataFrame)
# target_link_libraries(gui PRIVATE libcoro)
target_link_libraries(gui PRIVATE BT::behaviortree_cpp)
target_link_libraries(gui PRIVATE EnTT::EnTT)
target_link_libraries(gui PRIVATE mimalloc-static)
install(TARGETS gui DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(IMPORTED_RUNTIME_ARTIFACTS glfw DESTINATION ${CMAKE_INSTALL_PREFIX})
