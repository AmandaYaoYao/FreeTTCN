#
# Copyright (C) 2007 Mateusz Pusz
#
# This file is part of freettcn (Free TTCN) usage example.

# This example is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This example is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this example; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

include $(TOP_DIR)/Makefile.defines

# files
CPP_FILES = main.cpp

# defines
INCLUDE_DIRS = -I$(LIB_DIR)/src/include -Iinclude

# macros
O_FILES = $(addprefix $(BIN_DIR)/, $(patsubst %.cpp,%.o,$(CPP_FILES)))
DEPEND_FILES = $(patsubst %.cpp,%.d,$(CPP_FILES))

# modules
MODULES_O_FILES = $(addprefix $(BIN_DIR)/, $(patsubst %.ttcn,%.o,$(MOD_FILES)))

# libraries
INCLUDE_LIBS = $(addprefix -l, $(LIB_DIRS))



# targets
.PHONY: all tags clobber clean

all: $(O_FILES)
	$(call cmd,cmd_link)

tags:
	$(TAGS) --append --output=$(TAG_DIR)/TAGS *.cpp

clobber: clean
	$(Q)$(RM) -f *~ include/*~
	$(Q)$(RM) -f $(BIN_DIR)/$(PROG_NAME)

clean:
	$(Q)$(RM) -f $(LIB_PATH) $(O_FILES) $(DEPEND_FILES)
	$(Q)$(RM) -f $(BIN_DIR)/$(PROG_NAME)


# rules
$(BIN_DIR)/%.o: %.cpp
	$(call cmd,cmd_cc_d_o)

# include depends files
-include $(CPP_FILES:.cpp=.d)