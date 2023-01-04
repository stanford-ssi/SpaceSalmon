## Quail Remote HITL
Here's a quick guide on how to remotely connect to the Quail at my house and play with it:

### Step 1: Connect to my Raspberry Pi
```sh    
ssh -L 8080:localhost:8080 ssi@redmond.vrakaslabs.net -p 8022
```

This command connects on port 8022 to the Pi, and also opens a tunnel allowing local connections to port 8080 to be forwarded over the SSH tunnel to the Pi, so you can access the Web UI. To get the password, DM @timv on Slack.

### Step 2: Build the latest firmware:
```sh
cd ~/SpaceSalmon/ # go into the firmware project folder
git checkout master # select the master branch
git pull origin_https master # pull the latest code
. ~/.platformio/penv/bin/activate # activate the PlatformIO virtualenv
pio run -e quail -t upload # build and upload the Quail firmware over JTAG
```

### Step 3: run the GUI server
```sh
cd ~/Olympus-GroundControl/ # go into the ground software project folder
git checkout main # select the main branch
git pull origin_https main # pull the latest code
. venv/bin/activate # activate the server virtualenv
adev runserver main.py --port 8080 --livereload # start up the server
```

### Go to http://localhost:8080/ to play with the GUI.
The connection to localhost gets redirected over SSH to the Pi!
The password to send commands is `MAGIC`
