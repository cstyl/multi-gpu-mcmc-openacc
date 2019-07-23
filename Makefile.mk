##############################################################################
#
#  Makefile.mk
#
#  Please copy one configuration file from the ./config
#  directory to this directory (top level mcmc directory)
#  and make any appropriate changes for your platform.
#
#  No changes should be required in this file itself.
#
##############################################################################

ROOT_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

include $(ROOT_DIR)/config.mk
