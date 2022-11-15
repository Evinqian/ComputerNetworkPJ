from testlib import * 
import re

r1 = Runner("./ftp", "15441")
r2 = Runner("./ftp-server", ["localhost", "15441"])

@test(20, title="Hello world")
def test_hello():
    r.match("Hello world")

# TODO
# 编写test函数

if __name__ == "__main__":
    run_tests()

