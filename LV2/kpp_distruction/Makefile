	# check if user is root
	user = $(shell whoami)
	ifeq ($(user),root)
	INSTALL_LV2_DIR = /usr/lib/lv2
	INSTALL_LADSPA_DIR = /usr/lib/ladspa
	else 
	INSTALL_LV2_DIR = ~/.lv2
	INSTALL_LADSPA_DIR = ~/.ladspa
	endif

	# set bundle name
	NAME = kpp_distruction
	BUNDLE = $(NAME).lv2
	VER = 1.0
	# set compile flags
	CXXFLAGS += -shared -pthread -O2 -fPIC -Wl,--no-as-needed 
	LDFLAGS +=  -lm -I. '-DDLLEXT=".so"' '-DPLUGIN_URI="https://faustlv2.bitbucket.io/$(NAME)"' -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=0 -DVOICE_CTRLS=1 
	GUI_LDFLAGS += -I. -lm `pkg-config --cflags --libs cairo` -L/usr/X11/lib -lX11  -DPLUGIN_URI=\"https://faustlv2.bitbucket.io/$(NAME)\" -DFAUST_META=1 -DFAUST_MIDICC=1 -DFAUST_MTS=1 -DFAUST_UI=1 -DVOICE_CTRLS=1
	# invoke build files
	OBJECTS = faust-generated/$(NAME).cpp 
	GUI_OBJECTS = $(NAME)_ui.src/$(NAME)_gui.c
	## output style (bash colours)
	BLUE = "\033[1;34m"
	RED =  "\033[1;31m"
	NONE = "\033[0m"

.PHONY : all faust clean gui nogui LV2 LADSPA install uninstall 

all : gui
	@mkdir -p ./$(BUNDLE)
	@mv ./*.so ./$(BUNDLE)
	@if [ -f ./$(BUNDLE)/$(NAME).so ]; then echo $(BLUE)"build finish, now run make install"; \
	else echo $(RED)"sorry, build failed"; fi
	@echo $(NONE)

LADSPA :
	@faust2ladspa ../../LADSPA/$(NAME)/$(NAME).dsp

LV2 : all

faust:
	@echo $(BLUE)"regenerate from faust dsp file"$(NONE)
	@mkdir -p ./faust-temp
	@cp $(NAME).dsp  faust-temp
	@faust2lv2 -keep ./faust-temp/$(NAME).dsp
	@cp ./faust-temp/$(NAME)/$(NAME).cpp ./faust-generated
	@rm -rf faust-temp
	@echo ". ." $(BLUE)", done"$(NONE)

clean :
	@rm -f ./$(BUNDLE)/$(NAME).so
	@rm -f ./$(BUNDLE)/$(NAME)ui.so
	@rm -f ../../LADSPA/$(NAME)/$(NAME).so
	@echo $(BLUE)"clean up"$(NONE)

install :
	@if [ -f ../../LADSPA/$(NAME)/$(NAME).so ]; then \
	echo $(BLUE)"install LADSPA version to $(INSTALL_LADSPA_DIR)"; \
	mkdir -p $(DESTDIR)$(INSTALL_LADSPA_DIR); \
	cp -r ../../LADSPA/$(NAME)/$(NAME).so $(DESTDIR)$(INSTALL_LADSPA_DIR)/$(NAME).so; fi
ifneq ("$(wildcard ./$(BUNDLE))","")
	@mkdir -p $(DESTDIR)$(INSTALL_LV2_DIR)/$(BUNDLE)
	@echo $(BLUE)"install LV2 version to $(INSTALL_LV2_DIR)"$(NONE)
	@cp -r ./$(BUNDLE)/* $(DESTDIR)$(INSTALL_LV2_DIR)/$(BUNDLE)
else
	@echo ". ." $(BLUE)", you must build first"$(NONE)
endif

uninstall :
	@rm -rf $(INSTALL_LV2_DIR)/$(BUNDLE)
	@rm -rf $(INSTALL_LADSPA_DIR)/$(NAME).so
	@echo ". ." $(BLUE)", done"$(NONE)

gui : 
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(NAME).so
	$(CXX) $(CXXFLAGS) $(GUI_OBJECTS) $(GUI_LDFLAGS) -o $(NAME)ui.so

nogui :
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(NAME).so
