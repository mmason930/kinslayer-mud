CC = g++

FLUSSPFERD_INCLUDES = ../include
# Any special flags you want to pass to the compiler
MYFLAGS =  -O -std=c++0x -DXP_UNIX -I/usr/local/include -DICONV_ACCEPTS_NONCONST_INPUT=1 -Wformat-security -Wformat -Wpointer-arith -Wcast-align -Wredundant-decls \
 		-L/usr/local/lib -DJS_C_STRINGS_ARE_UTF8 -DBOOST_PARAMETER_MAX_ARITY=10 \
              -I/usr/local/lib/pkgconfig/../../include -I$(FLUSSPFERD_INCLUDES) -I/usr/local/include

#flags for profiling (see hacker.doc for more information)
PROFILE =

##############################################################################
# Do Not Modify Anything Below This Line (unless you know what you're doing) #
##############################################################################

CPPFLAGS = -ggdb3 $(MYFLAGS) $(PROFILE)

LIBS = 

OBJDIR = obj
LIBDIR = ../lib

OBJFILES = arguments.o array.o binary.o binary_stream.o class.o context.o convert.o create.o encodings.o evaluate.o \
exception.o file.o filesystem-base.o flusspferd_module.o function.o function_adapter.o getopt.o init.o \
io.o json2.o load_core.o local_root_scope.o modules.o native_function_base.o native_object_base.o object.o \
properties_functions.o property_attributes.o property_iterator.o root.o security.o stream.o string.o system.o \
tracer.o value.o

OBJFILES := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJFILES))

$(OBJDIR)/%.d: %.cpp
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< | \
	sed '\''s,\($*\)\.o[ :]*,$(OBJDIR)\/\1.o $@ : ,g'\'' > $@'

default: all

-include $(OBJFILES:.o)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) -c	$<	-o $@

all:
	$(MAKE) flusspferd

flusspferd: $(OBJFILES)
	ar rc $(LIBDIR)/libflusspferd.a $(OBJFILES)

clean:
	rm -f $(OBJFILES) *~
	rm -f $(LIBDIR)/*

install:
	cp $(LIBDIR)/* /usr/local/lib/
#	rm -rf /usr/local/include/flusspferd
	cp -R $(FLUSSPFERD_INCLUDES)/* /usr/local/include/

full:
	clean
	all
