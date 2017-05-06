#!/bin/bash

source /pkgscripts/include/pkg_util.sh

package="liblog"
version="1.0"
displayname="liblog"
maintainer="dfgs"
arch="$(pkg_get_unified_platform)"
description="log library dependency for WORM project"
[ "$(caller)" != "0 NULL" ] && return 0
pkg_dump_info
