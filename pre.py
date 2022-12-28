#generate build version!
from datetime import datetime
import getpass
import platform
import subprocess

from pathlib import Path
import pkg_resources

Import("env")

requirements = pkg_resources.parse_requirements(Path("requirements.txt").open())
for requirement in requirements:
    try:
        requirement = str(requirement)
        pkg_resources.require(requirement)
    except pkg_resources.ResolutionError:
        env.Execute(
            env.Action(
                f'$PYTHONEXE -m pip install "{requirement}"',
                f"Installing python dependencies: {requirement}",
            )
        )   

from src.slate.slate import generate_slate

generate_slate("src/dummy/telemetry.yaml","src/dummy/generated/")
generate_slate("src/quail/telemetry.yaml","src/quail/generated/")


git_attr = subprocess.check_output(['git', 'describe', '--long', '--dirty', '--tags']).decode("utf-8").strip()
time_attr = datetime.utcnow().strftime('%d/%m/%Y-%H:%M')
user_attr = getpass.getuser()
platform_attr = platform.uname()[1]

buildstr =  f"{git_attr}--{user_attr}@{platform_attr}--{time_attr}"
print("BUILD VERSION: " + buildstr)

#save it to the version.cpp file
with open('src/version.cpp','w') as f:
    f.write('''//This is an auto-generated file, do not edit!
//When the project builds, this file is updated with the build version by a python script
const char* build_version = \"{}\";\
'''.format(buildstr))
