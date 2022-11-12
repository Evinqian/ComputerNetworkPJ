from testlib import * 
import re

r = Runner("./ftp")

@test(20, title="Hello world")
def test_hello():
    r.match("Hello world")

# TODO
# 编写test函数

if __name__ == "__main__":
    run_tests()

