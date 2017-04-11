NAME = driver

include $(ROOTDIR)/Rules.make


SUB_DIRS = $(shell ls  -F | grep /$ | grep -v Debug | grep -v Release | \
		             grep -v CVS | grep -v arch | grep -v include | grep -v obj \
					         | sed "s/\///g")

SUB_MODULE = $(addsuffix /$(NAME).o ,$(SUB_DIRS))

all: $(NAME).o

$(NAME).o: $(SUB_MODULE)
	$(LD) -r $(LDFLAGS)  -o $(NAME).o  $(SUB_MODULE)
