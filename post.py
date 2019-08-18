Import("env","projenv")

from datetime import datetime
import getpass
import platform
import subprocess

git_attr = subprocess.check_output(['git', 'describe', '--long', '--dirty', '--tags']).decode("utf-8").strip()
time_attr = datetime.utcnow().strftime('%d/%m/%Y-%H:%M')
user_attr = getpass.getuser()
platform_attr = platform.uname()[1]

buildstr =  f"{git_attr}--{user_attr}@{platform_attr}--{time_attr}"
print("BUILD VERSION: " + buildstr)
projenv.Append(CPPDEFINES=[("PIO_BUILD", "\\\"" + buildstr + "\\\"")])