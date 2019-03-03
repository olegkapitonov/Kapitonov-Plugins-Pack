	# check if user is root
	user = $(shell whoami)
	ifeq ($(user),root)
	INSTALL_LV2_DIR = /usr/lib/lv2
	else 
	INSTALL_LV2_DIR = ~/.lv2
	endif

	# set bundle name
	NAME = kpp_tubeamp
	BUNDLE = $(NAME).lv2
	VER = 1.0
	# set compile flags
	CXXFLAGS += -shared -pthread -O2 -Wall -fPIC -Wl,--no-as-needed 
	LDFLAGS += -lfftw3 -lfftw3f -lm -I. '-DDLLEXT=".so"' '-DPLUGIN_URI="https://faustlv2.bitbucket.io/$(NAME)"' -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=0 -DVOICE_CTRLS=1 
	GUI_LDFLAGS += -I. -lm `pkg-config --cflags --libs cairo` -L/usr/X11/lib -lX11  -DPLUGIN_URI=\"https://faustlv2.bitbucket.io/$(NAME)\" -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=1 -DVOICE_CTRLS=1
	# invoke build files
	OBJECTS = $(NAME).cpp 
	GUI_OBJECTS = $(NAME)_ui.src/$(NAME)_gui.c
	## output style (bash colours)
	BLUE = "\033[1;34m"
	RED =  "\033[1;31m"
	NONE = "\033[0m"

.PHONY : all faust clean nogui LV2 LADSPA install uninstall 

all :  $(NAME)
	@mkdir -p ./$(BUNDLE)
	@mv ./*.so ./$(BUNDLE)
	@if [ -f ./$(BUNDLE)/$(NAME).so ]; then echo $(BLUE)"build finish, now run make install"; \
	else echo $(RED)"sorry, build failed"; fi
	@echo $(NONE)

LADSPA :
	@echo $(BLUE)"LADSPA pass"$(NONE)

LV2 : all

faust:
	@echo $(BLUE)"regenerate from faust dsp file"$(NONE)
	@faust kpp_tubeamp.dsp >faust-generated/$(NAME)_dsp.cpp
	@cd faust-generated; patch < stPorts.patch
	@echo ". ." $(BLUE)", done"$(NONE)

clean :
	@rm -f ./$(BUNDLE)/$(NAME).so
	@rm -f ./$(BUNDLE)/$(NAME)ui.so
	@echo $(BLUE)"clean up"$(NONE)

install :
ifneq ("$(wildcard ./$(BUNDLE))","")
	@mkdir -p $(DESTDIR)$(INSTALL_LV2_DIR)/$(BUNDLE)
	cp -r ./$(BUNDLE)/* $(DESTDIR)$(INSTALL_LV2_DIR)/$(BUNDLE)
	@echo ". ." $(BLUE)", done"$(NONE)
else
	@echo ". ." $(BLUE)", you must build first"$(NONE)
endif

uninstall :
	@rm -rf $(INSTALL_LV2_DIR)/$(BUNDLE)
	@echo ". ." $(BLUE)", done"$(NONE)

$(NAME) :
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(NAME).so
	$(CXX) $(CXXFLAGS)  $(GUI_OBJECTS) $(GUI_LDFLAGS) -o $(NAME)ui.so

nogui :
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(NAME).so
