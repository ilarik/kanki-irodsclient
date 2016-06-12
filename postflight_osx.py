#!/usr/bin/python

from __future__ import absolute_import
from __future__ import print_function
from __future__ import unicode_literals

import datetime
import shutil
import pytz
import json
import os

irods_environment_path = os.path.join(os.getenv("HOME"), ".irods", "irods_environment.json")
backup_file_path = irods_environment_path + ".backup-" + (datetime.datetime.now(pytz.utc).strftime("%Y-%m-%dT%H:%M:%S%Z"))

if (os.path.exists(irods_environment_path)):
    shutil.copyfile(irods_environment_path, backup_file_path)
    irods_environment_file = open(irods_environment_path, 'r')
    try:
        irods_environment_json = json.load(irods_environment_file)
    except ValueError:
        irods_environment_json = {}
else:
    irods_environment_json = {}

irods_environment_json["irods_plugins_home"] = "/Applications/iRODS.app/Contents/PlugIns/irods/"

with open(irods_environment_path, 'w+') as irods_environment_file:
    output = json.dump(irods_environment_json, irods_environment_file, indent=4, sort_keys=True)
