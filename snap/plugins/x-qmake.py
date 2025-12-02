# -*- Mode:Python; indent-tabs-mode:nil; tab-width:4 -*-
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright (C) 2013 - 2024, nymea GmbH
# Copyright (C) 2024 - 2025, chargebyte austria GmbH
#
# This file is part of nymea-networkmanager.
#
# nymea-networkmanager is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# nymea-networkmanager is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with nymea-networkmanager. If not, see <https://www.gnu.org/licenses/>.
#
# Additional original work:
# Copyright (C) 2016 Canonical Ltd
# Copyright (C) 2017 Simon Stürz <simon.stuerz@guh.io>

"""The qmake plugin is useful for building qmake-based parts.

These are projects that are built using .pro files.

This plugin uses the common plugin keywords as well as those for "sources".
For more information check the 'plugins' topic for the former and the
'sources' topic for the latter.

Additionally, this plugin uses the following plugin-specific keywords:

    - options:
      (list of strings)
      additional options to pass to the qmake invocation.
    - project-files:
      (list of strings)
      list of .pro files to pass to the qmake invocation.
"""

import os

import snapcraft
from snapcraft import common


class XQmakePlugin(snapcraft.BasePlugin):

    @classmethod
    def schema(cls):
        schema = super().schema()
        schema['properties']['options'] = {
            'type': 'array',
            'minitems': 1,
            'uniqueItems': True,
            'items': {
                'type': 'string',
            },
            'default': [],
        }
        schema['properties']['project-files'] = {
            'type': 'array',
            'minitems': 1,
            'uniqueItems': True,
            'items': {
                'type': 'string',
            },
            'default': [],
        }

        return schema

    @classmethod
    def get_build_properties(cls):
        # Inform Snapcraft of the properties associated with building. If these
        # change in the YAML Snapcraft will consider the build step dirty.
        return ['options', 'project-files']

    def __init__(self, name, options, project):
        super().__init__(name, options, project)

        self.build_packages.append('make')

    def build(self):
        super().build()

        env = self._build_environment()

        sources = []
        if self.options.project_files:
            sourcedir = self.sourcedir
            source_subdir = getattr(self.options, 'source_subdir', None)
            if source_subdir:
                sourcedir = os.path.join(sourcedir, source_subdir)
            sources = [os.path.join(sourcedir, project_file)
                       for project_file in self.options.project_files]

        #self.run(['qmake'] + self._extra_config() + self.options.options + sources, env=env)
        self.run([ self.project.parts_dir + '/qt-5-11-1/install/bin/qmake'] + self._extra_config() + self.options.options + sources, env=env)

        self.run(['make', '-j{}'.format( self.parallel_build_count)], env=env)

        self.run(['make', 'install', 'INSTALL_ROOT=' + self.installdir], env=env)

    def _extra_config(self):
        extra_config = []

        for root in [self.installdir, self.project.stage_dir]:
            paths = common.get_library_paths(root, self.project.arch_triplet)
            for path in paths:
                extra_config.append("LIBS+=\"-L{}\"".format(path))
            extra_config.append("LIBS+=\"-L{}\"".format(self.project.parts_dir + '/qt-5-11-1/install/lib'))
            extra_config.append("QMAKE_LIBS+=\"-L{}\"".format(self.project.parts_dir + '/qt-5-11-1/install/lib'))
            extra_config.append("QMAKE_LIBDIR+=\"{}\"".format(self.project.parts_dir + '/qt-5-11-1/install/lib'))

            paths = common.get_include_paths(root, self.project.arch_triplet)
            for path in paths:
                extra_config.append("INCLUDEPATH+=\"{}\"".format(path))
            extra_config.append("INCLUDEPATH+=\"{}\"".format(self.project.parts_dir + '/qt-5-11-1/install/include'))

            extra_config.append("QML_IMPORT_PATH+=\"{}\"".format(self.project.parts_dir + '/qt-5-11-1/install/qml'))
            extra_config.append("QML2_IMPORT_PATH+=\"{}\"".format(self.project.parts_dir + '/qt-5-11-1/install/qml'))

        return extra_config

    def _build_environment(self):
        env = os.environ.copy()
        env['QTDIR' ] = self.project.parts_dir + '/qt-5-11-1/install/'
        env['QML_IMPORT_PATH' ] = self.project.parts_dir + '/qt-5-11-1/install/qml'
        env['QML2_IMPORT_PATH' ] = self.project.parts_dir + '/qt-5-11-1/install/qml'
        env['LD_LIBRARY_PATH' ] = self.project.parts_dir + '/qt-5-11-1/install/lib:' + self.project.parts_dir + '/qt-5-11-1/install/usr/lib/:'
        env['PATH' ] = self.project.parts_dir + '/qt-5-11-1/install/bin:' + os.environ["PATH"]
        return env
