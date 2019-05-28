# DSQSS (Discrete Space Quantum Systems Solver)
# Copyright (C) 2018- The University of Tokyo
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from setuptools import setup

setup(
    name="dsqss",
    version="2.0.0-Beta",
    description="DSQSS input files generator",
    url="https://github.com/issp-center-dev/dsqss",
    author="DSQSS developers",
    author_email="dsqss-dev@issp.u-tokyo.ac.jp",
    license="GPLv3",
    packages=["dsqss", 'dsqss.lattice_factory'],
    python_requires=">=2.7",
    install_requires=["numpy", "scipy", "toml"],
    entry_points={
        "console_scripts": [
            "dla_hamgen = dsqss.std_model:main",
            "dla_latgen = dsqss.std_lattice:main",
            "dla_pgen = dsqss.parameter:main",
            "dla_wvgen = dsqss.wavevector:main",
            "dla_alg = dsqss.dla_alg:main",
            "dla_pre = dsqss.dla_pre:main",
            "pmwa_pre = dsqss.pmwa_pre:main",
        ]
    },
    zip_safe=False,
)
