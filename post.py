Import("env","projenv")

from datetime import datetime
import getpass
import platform
import subprocess

buildstr = subprocess.check_output(['git', 'describe', '--long', '--dirty', '--tags']).strip() + "-" + getpass.getuser() + "@" + platform.uname()[1] + "-" + datetime.utcnow().strftime('%d/%m/%Y-%H:%M')
print("BUILD VERSION: " + buildstr)
projenv.Append(CPPDEFINES=[("PIO_BUILD", "\\\"" + buildstr + "\\\"")])