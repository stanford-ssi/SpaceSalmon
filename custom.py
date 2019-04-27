Import("env")

env.Prepend(
    CCFLAGS=[
        "-mfloat-abi=hard",
        "-mfpu=fpv4-sp-d16"
    ],
    LINKFLAGS=[
        "-mfloat-abi=hard",
        "-mfpu=fpv4-sp-d16"
    ],
    LIBS=["arm_cortexM4lf_math"]
)

from datetime import datetime
import getpass
import platform
import subprocess

buildstr = subprocess.check_output(['git', 'describe', '--long', '--dirty', '--tags']).strip() + "-" + getpass.getuser() + "@" + platform.uname()[1] + "-" + datetime.utcnow().strftime('%d/%m/%Y-%H:%M')
print("BUILD VERSION: " + buildstr)
env.Append(CPPDEFINES=[("PIO_BUILD", "\\\"" + buildstr + "\\\"")])