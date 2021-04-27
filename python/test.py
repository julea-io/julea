# JULEA - Flexible storage framework
# Copyright (C) 2019-2020 Johannes Coym
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from julea.test import distributed_object_test, item_test, kv_test, object_test
from julea import juleadir
import subprocess

setupfile = juleadir.joinpath('scripts', 'setup.sh')

subprocess.run([setupfile, "start"])

object_test()
distributed_object_test()
item_test()
kv_test()

subprocess.run([setupfile, "stop"])
