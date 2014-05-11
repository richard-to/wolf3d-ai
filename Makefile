dir_guard=@mkdir -p
res_cp=@cp -R 

CXX=g++
INC=-Iinclude
LIB=-L/usr/lib -Llib -lX11 -lXtst -lopencv_core -lopencv_flann -lopencv_video
CXXFLAGS=-O0 -std=gnu++11 -g -Wall

LIBDIR=lib
BINDIR=bin
SRCDIR=src
RESDIR=res

CORE_MODULE=core
CORE_SRCDIR=$(SRCDIR)/$(CORE_MODULE)/
CORE_LIBDIR=$(LIBDIR)/$(CORE_MODULE)/
CORE_MKDIR=$(dir_guard) $(CORE_LIBDIR)
CORE_DEPS=$(CORE_LIBDIR)util.o $(CORE_LIBDIR)x11_controller.o $(CORE_LIBDIR)locate_enemies.o $(CORE_LIBDIR)locate_doors.o $(CORE_LIBDIR)localize.o

EXPERIMENTS_MODULE=experiments
EXPERIMENT_EXEC=x11_screen_grab x11_send_event image_scale_up image_scale_down gaussian_blur sift cv_sift_door_finder door_segment
EXPERIMENTS_SRCDIR=$(SRCDIR)/$(EXPERIMENTS_MODULE)/
EXPERIMENTS_LIBDIR=$(LIBDIR)/$(EXPERIMENTS_MODULE)/
EXPERIMENTS_BINDIR=$(BINDIR)/$(EXPERIMENTS_MODULE)/
EXPERIMENTS_RESDIR=$(RESDIR)/$(EXPERIMENTS_MODULE)/
EXPERIMENT_MKDIR=$(dir_guard) $(EXPERIMENTS_BINDIR) $(EXPERIMENTS_LIBDIR)
EXPERIMENT_RESCP=$(res_cp) $(EXPERIMENTS_RESDIR)/* $(EXPERIMENTS_BINDIR)

define compile_core
	$(CORE_MKDIR)
	$(CXX) $(INC) $(CXXFLAGS) -c $(basename $(subst $(CORE_LIBDIR), $(CORE_SRCDIR), $@)).cpp -o $@
endef

define make_experiments
	$(EXPERIMENT_MKDIR)
	$(EXPERIMENT_RESCP)
	$(CXX) $(INC) $(CXXFLAGS) -c $(EXPERIMENTS_SRCDIR)$@.cpp -o $(EXPERIMENTS_LIBDIR)$@.o	
	$(CXX) $(INC) $(CXXFLAGS) -o $(EXPERIMENTS_BINDIR)$@ $(EXPERIMENTS_LIBDIR)$@.o $^ `pkg-config --libs opencv` $(LIB)
endef

all: $(EXPERIMENT_EXEC)

x11_screen_grab:
	$(call make_experiments)

x11_send_event:
	$(call make_experiments)

image_scale_up:
	$(call make_experiments)

image_scale_down:
	$(call make_experiments)

gaussian_blur:
	$(call make_experiments)

cv_sift_door_finder:
	$(call make_experiments)

sift:
	$(call make_experiments)

door_segment:
	$(call make_experiments)

door_segment2: $(CORE_LIBDIR)util.o $(CORE_LIBDIR)locate_doors.o
	$(call make_experiments)

enemy_segment: $(CORE_LIBDIR)util.o $(CORE_LIBDIR)locate_enemies.o
	$(call make_experiments)

wall_measure: $(CORE_LIBDIR)localize.o
	$(call make_experiments)

reorient:
	$(call make_experiments)

wolf3d_ai: $(CORE_DEPS)
	$(CXX) $(INC) $(CXXFLAGS) -c $(SRCDIR)/$@.cpp -o $(LIBDIR)/$@.o
	$(CXX) $(INC) $(CXXFLAGS) -o $(BINDIR)/$@ $(LIBDIR)/$@.o $^ `pkg-config --libs opencv` $(LIB)

$(CORE_LIBDIR)x11_controller.o: $(CORE_LIBDIR)util.o
	$(call compile_core)

$(CORE_LIBDIR)locate_enemies.o: $(CORE_LIBDIR)util.o
	$(call compile_core)

$(CORE_LIBDIR)locate_doors.o: $(CORE_LIBDIR)util.o
	$(call compile_core)

$(CORE_LIBDIR)localize.o:
	$(call compile_core)

$(CORE_LIBDIR)util.o:
	$(call compile_core)

clean:
	-rm -rf bin/*
	-rm -rf lib/*	