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

DEPS=x11_controller.o locate_enemies.o locate_doors.o localize.o util.o
CORE_MODULE=core
CORE_EXEC=x11_controller
CORE_SRCDIR=$(SRCDIR)/$(CORE_MODULE)/
CORE_LIBDIR=$(LIBDIR)/$(CORE_MODULE)/
CORE_MKDIR=$(dir_guard) $(CORE_LIBDIR)

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
	$(CXX) $(INC) $(CXXFLAGS) -c $(CORE_SRCDIR)$(basename $@).cpp -o $(CORE_LIBDIR)$@
endef

define make_experiments
	$(EXPERIMENT_MKDIR)
	$(EXPERIMENT_RESCP)
	$(CXX) $(CXXFLAGS) -c $(EXPERIMENTS_SRCDIR)$@.cpp -o $(EXPERIMENTS_LIBDIR)$@.o	
	$(CXX) $(CXXFLAGS) -o $(EXPERIMENTS_BINDIR)$@ $(EXPERIMENTS_LIBDIR)$@.o `pkg-config --libs opencv` $(LIB)
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

wolf3d_ai: $(DEPS)
	$(CXX) $(INC) $(CXXFLAGS) -c $(SRCDIR)/$@.cpp -o $(LIBDIR)/$@.o

x11_controller.o: util.o
	$(call compile_core)

locate_enemies.o: util.o
	$(call compile_core)

locate_doors.o: util.o
	$(call compile_core)

localize.o:
	$(call compile_core)

util.o:
	$(call compile_core)


clean:
	-rm -rf bin/*
	-rm -rf lib/*
	