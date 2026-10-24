# run68でHuman68kの実行ファイルを実行し、標準出力を期待出力と比較する。
#   cmake -DRUN68=<run68> -DPROGRAM=<.x> -DEXPECTED=<期待出力> -P run_test.cmake
# 作業ディレクトリは呼び出し側(add_testのWORKING_DIRECTORY)で指定する。

execute_process(
  COMMAND "${RUN68}" "${PROGRAM}"
  OUTPUT_VARIABLE actual
  RESULT_VARIABLE result)

if(NOT result EQUAL 0)
  message(FATAL_ERROR "run68 exited with ${result}")
endif()

file(READ "${EXPECTED}" expected)
if(NOT actual STREQUAL expected)
  message(FATAL_ERROR "output mismatch\n--- expected\n${expected}\n--- actual\n${actual}")
endif()
