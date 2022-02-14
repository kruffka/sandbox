cmake_minimum_required(VERSION 2.8)	 # Проверка версии CMake.
										# Если версия установленой программы
										# старее указаной, произайдёт аварийный выход.

project(structures)			# Название проекта

SET(CMAKE_C_FLAGS "-Wall")

add_compile_options(${CMAKE_C_FLAGS})
message(STATUS "added compile options: ${CMAKE_C_FLAGS}")

set(SOURCE main.c)		# Установка переменной со списком исходников для исполняемого файла

add_executable(main ${SOURCE})	# Создает исполняемый файл с именем main

target_link_libraries(main)		# Линковка программы с библиотекой