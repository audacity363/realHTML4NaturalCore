#CC = /usr/vac/bin/xlc
CC = /usr/bin/gcc
AR = /usr/bin/ar

#XLC:
LFLAGS1_SO = -G
LFLAGS2_SO = 

#GCC:
LFLAGS1_SO = -shared 
LFLAGS2_SO = 

INCLUDE = -I./include/ \
		  -I./libs/rh4n_utils/include \
          -I./libs/rh4n_logging/include/ \
		  -I./libs/rh4n_vars/include/ \
		  -I./libs/rh4n_jsongenerator/include/ \
		  -I./libs/rh4n_ldaparser/include/ \
		  -I./libs/rh4n_var2name/include/ \
		  -I./libs/rh4n_messaging/include/ \
		  -I./natuser/include/ \
		  -I./natcaller/include/

LIBS = -L./bin/libs \
	   -lrh4nmessaging -lrh4njsongenerator -lrh4nutils -lrh4nlogging -lrh4nvar2name -lrh4nvars -lrh4nldaparser \
	   -ldl

#XLC:
CARGS1 = -g -c -fpic $(INCLUDE)
CARGS2 = 
CARGS_SO = -c -g -fpic $(INCLUDE)

#GCC:
CARGS1 = -g -c -Wall -fpic $(INCLUDE)
#Save preprocessor and assembly files 
#CARGS1 = -g -c -Wall -fpic $(INCLUDE) -save-temps=obj
CARGS2 = 
CARGS_SO = -c -g -fpic $(INCLUDE)

LIBOUTPUT = ./bin/libs/
TESTOUTPUT = ./bin/tests/

help:
	@printf "Targets:\n"
	@printf "\t+----------------------------CORE:---------------------------+\n"
	@printf "\t| utils                                                      |\n"
	@printf "\t|   Utils library (./libs/rh4n_utils)                        |\n"
	@printf "\t|                                                            |\n"
	@printf "\t| logging                                                    |\n"
	@printf "\t|   Logging library (./libs/rh4n_logging)                    |\n"
	@printf "\t|                                                            |\n"
	@printf "\t| vars                                                       |\n"
	@printf "\t|   Vars library (./libs/rh4n_vars)                          |\n"
	@printf "\t|                                                            |\n"
	@printf "\t| jsongenerator                                              |\n"
	@printf "\t|   JSON Generator library (./libs/rh4n_jsongenerator)       |\n"
	@printf "\t|                                                            |\n"
	@printf "\t| ldaparser                                                  |\n"
	@printf "\t|   LDA parser library (./libs/rh4n_ldaparser)               |\n"
	@printf "\t|                                                            |\n"
	@printf "\t| var2name                                                   |\n"
	@printf "\t|   Name and Postion matching library (./libs/rh4n_var2name) |\n"
	@printf "\t|                                                            |\n"
	@printf "\t| messaging                                                  |\n"
	@printf "\t|   Communication between the framework and other components |\n"
	@printf "\t|   (./libs/rh4n_messaging)                                  |\n"
	@printf "\t|                                                            |\n"
	@printf "\t| natuserlib                                                 |\n"
	@printf "\t|   Shared library for natural (./natuserlib)                |\n"
	@printf "\t|                                                            |\n"
	@printf "\t| natcaller                                                  |\n"
	@printf "\t|   Binary that calls natural (bug from Nat >8) (./natcaller)|\n"
	@printf "\t+------------------------------------------------------------+\n\n"
	@printf "\tall: Compiles everything\n"


all: utils logging vars var2name ldaparser messaging natuserlib natcaller
	@printf "You find the binarys under ./bin\n"

tests_pre:
	@mkdir -p $(TESTOUTPUT)
	@rm -f $(TESTOUTPUT)/*

#                         +------------------+
#-------------------------|      Utils       |---------------------------------
#                         +------------------+

UTILS_SRC = ./libs/rh4n_utils/src
UTILS_BIN = ./bin/rh4n_utils
UTILS_OBJS = rh4n_utils.o \
			 rh4n_utils_prop.o \
			 rh4n_utils_namestack.o
UTILS_LIB = librh4nutils.a

utils: utils_clean utils_pre $(UTILS_OBJS)
	@printf "Creating $(LIBOUTPUT)/$(UTILS_LIB)\n"
	@$(AR) -cru $(LIBOUTPUT)/$(UTILS_LIB) $(UTILS_BIN)/*.o
	@printf "Done compiling and linking utils\n"

$(UTILS_OBJS):
	@printf "CC $(UTILS_SRC)/$*.c => $(UTILS_BIN)/$*.o\n"
	@$(CC) $(CARGS1) -o $(UTILS_BIN)/$*.o $(UTILS_SRC)/$*.c

utils_pre:
	@printf "Creating utils output folder\n"
	@mkdir -p $(UTILS_BIN)
	@mkdir -p $(LIBOUTPUT)

utils_clean:
	@printf "Cleaning utils library\n"
	@rm -f $(LIBOUTPUT)/$(UTILS_BIN)
	@printf "Cleaning utils objects\n"
	@rm -f $(UTILS_BIN)/*.o

utils_tests: utils tests_pre
	@printf "Building utils tests\n"
	@$(CC) -Wall $(UTILS_SRC)/../tests/nameStack_test.c -o $(TESTOUTPUT)/nameStack_test $(INCLUDE) $(LIBS)

#                         +-----------------+
#-------------------------|     Logging     |----------------------------------
#                         +-----------------+

LOGGING_SRC = ./libs/rh4n_logging/src
LOGGING_BIN = ./bin/rh4n_logging
LOGGING_OBJS = rh4n_logging.c
LOGGING_LIB = librh4nlogging.a

logging: logging_clean logging_pre $(LOGGING_OBJS)
	@printf "Creating $(LIBOUTPUT)/$(LOGGING_LIB)\n"
	@$(AR) -cr $(LIBOUTPUT)/$(LOGGING_LIB) $(LOGGING_BIN)/*.o
	@printf "Done compiling and linking logging\n"

$(LOGGING_OBJS):
	@printf "CC $(LOGGING_SRC)/$*.c => $(LOGGING_BIN)/$*.o\n"
	@$(CC) $(CARGS1) -o $(LOGGING_BIN)/$*.o $(LOGGING_SRC)/$*.c

logging_pre:
	@printf "Creating logging output folder\n"
	@mkdir -p $(LOGGING_BIN)
	@mkdir -p $(LIBOUTPUT)

logging_clean:
	@printf "Cleaning logging library\n"
	@rm -f $(LIBOUTPUT)/$(UTILS_BIN)
	@printf "Cleaning logging objects\n"
	@rm -f $(LOGGING_BIN)/*.o


#                         +-----------------+
#-------------------------|      Vars       |----------------------------------
#                         +-----------------+

VARS_SRC = ./libs/rh4n_vars/src
VARS_BIN = ./bin/rh4n_vars
VARS_OBJS = rh4n_vars.o \
		    rh4n_vars_group.o \
		    rh4n_vars_print.o \
		    rh4n_vars_string.o \
		    rh4n_vars_array.o \
		    rh4n_vars_bool.o \
		    rh4n_vars_integer.o \
		    rh4n_vars_float.o \
			rh4n_vars_null.o \
		    rh4n_vars_ustring.o \
		    rh4n_vars_free.o \
			rh4n_vars_dump.o \
			rh4n_vars_load.o
VARS_LIB = librh4nvars.a

vars: logging vars_clean vars_pre $(VARS_OBJS)
	@printf "Creating $(LIBOUTPUT)/$(VARS_LIB)\n"
	@$(AR) -cr $(LIBOUTPUT)/$(VARS_LIB) $(VARS_BIN)/*.o
	@printf "Done compiling and linking vars\n"

$(VARS_OBJS):
	@printf "CC $(VARS_SRC)/$*.c => $(VARS_BIN)/$*.o\n"
	@$(CC) $(CARGS1) -o $(VARS_BIN)/$*.o $(VARS_SRC)/$*.c

vars_pre:
	@printf "Creating vars output folder\n"
	@mkdir -p $(VARS_BIN)
	@mkdir -p $(LIBOUTPUT)

vars_clean:
	@printf "Cleaning vars library\n"
	@rm -f $(LIBOUTPUT)/$(VARS_LIB)
	@printf "Cleaning vars objects\n"
	@rm -f $(VARS_BIN)/*.o

vars_test: var2name
	@$(CC) -g ./libs/rh4n_vars/test/main.c $(INCLUDE) -o ./libs/rh4n_vars/test/main $(LIBS)
	@./libs/rh4n_vars/test/main

vars_file_dump: vars
	@$(CC) -g ./libs/rh4n_vars/test/var_dump.c $(INCLUDE) -o ./libs/rh4n_vars/test/var_dump $(LIBS)
	@./libs/rh4n_vars/test/var_dump

vars_file_load: vars
	@$(CC) -g ./libs/rh4n_vars/test/var_load.c $(INCLUDE) -o ./libs/rh4n_vars/test/var_load $(LIBS)
	@./libs/rh4n_vars/test/var_load

vars_tests: vars tests_pre
	@printf "Building vars tests\n"
	@$(CC) -Wall -g $(VARS_SRC)/../tests/array_test.c -o $(TESTOUTPUT)/array_test $(INCLUDE) $(LIBS) -lrh4nlogging -lrh4nvars

#                         +-----------------+
#-------------------------|  JSON generator |----------------------------------
#                         +-----------------+

JSON_SRC = ./libs/rh4n_jsongenerator/src
JSON_BIN = ./bin/rh4n_jsongenerator
JSON_OBJS = rh4n_json_generator.o \
			rh4n_json_array.o \
			rh4n_json_group.o
JSON_LIB = librh4njsongenerator.a

json_generator: logging vars json_generator_clean json_generator_pre $(JSON_OBJS)
	@printf "Creating $(LIBOUTPUT)/$(JSON_LIB)\n"
	@$(AR) -cr $(LIBOUTPUT)/$(JSON_LIB) $(JSON_BIN)/*.o
	@printf "Done compiling and linking vars\n"

$(JSON_OBJS):
	@printf "CC $(JSON_SRC)/$*.c => $(JSON_BIN)/$*.o\n"
	@$(CC) $(CARGS1) -o $(JSON_BIN)/$*.o $(JSON_SRC)/$*.c

json_generator_pre:
	@printf "Creating json generator output folder\n"
	@mkdir -p $(JSON_BIN)
	@mkdir -p $(LIBOUTPUT)

json_generator_clean:
	@printf "Cleaning json generator library\n"
	@rm -f $(LIBOUTPUT)/$(JSON_BIN)
	@printf "Cleaning vars objects\n"
	@rm -f $(JSON_BIN)/*.o

json_test: json_generator
	@$(CC) -g ./libs/rh4n_jsongenerator/tests/main.c $(INCLUDE) -o ./libs/rh4n_jsongenerator/tests/main $(LIBS)
	@./libs/rh4n_jsongenerator/tests/main 2> ./jsonout
	@cat ./jsonout

json_generator_tests: json_generator tests_pre
	@printf "CC ./libs/rh4n_jsongenerator/tests/null.c => ./bin/tests/json_generator_null\n"
	@$(CC) -g ./libs/rh4n_jsongenerator/tests/null.c $(INCLUDE) -o ./bin/tests/json_generator_null $(LIBS)

#                         +-----------------+
#-------------------------|    LDA parser   |----------------------------------
#                         +-----------------+

LDAPARSER_SRC = ./libs/rh4n_ldaparser/src
LDAPARSER_BIN = ./bin/rh4n_ldaparser
LDAPARSER_OBJS = rh4n_ldaparser.o \
			     rh4n_lda_filehandling.o \
			     rh4n_lda_array.o \
			     rh4n_lda_utils.o \
			     rh4n_lda_variables.o \
			     rh4n_lda_print.o \
			     rh4n_lda_free.o
LDAPARSER_LIB = librh4nldaparser.a

ldaparser: logging ldaparser_clean ldaparser_pre $(LDAPARSER_OBJS)
	@printf "Creating $(LIBOUTPUT)/$(LDAPARSER_LIB)\n"
	@$(AR) -cr $(LIBOUTPUT)/$(LDAPARSER_LIB) $(LDAPARSER_BIN)/*.o
	@printf "Done compiling and linking ldaparser\n"

$(LDAPARSER_OBJS):
	@printf "CC $(LDAPARSER_SRC)/$*.c => $(LDAPARSER_BIN)/$*.o\n"
	@$(CC) $(CARGS1) -o $(LDAPARSER_BIN)/$*.o $(LDAPARSER_SRC)/$*.c

ldaparser_pre:
	@printf "Creating ldaparser output folder\n"
	@mkdir -p $(LDAPARSER_BIN)
	@mkdir -p $(LIBOUTPUT)

ldaparser_clean:
	@printf "Cleaning ldaparser library\n"
	@rm -f $(LIBOUTPUT)/$(UTILS_BIN)
	@printf "Cleaning ldaparser objects\n"
	@rm -f $(LDAPARSER_BIN)/*.o

ldaparser_test: ldaparser
	@$(CC) -g ./libs/rh4n_ldaparser/test/main.c $(INCLUDE) -o ./libs/rh4n_ldaparser/test/main $(LIBS)
	@./libs/rh4n_ldaparser/test/main

#                         +-----------------+
#-------------------------|    Var2Name     |----------------------------------
#                         +-----------------+

VAR2NAME_SRC = ./libs/rh4n_var2name/src
VAR2NAME_BIN = ./bin/rh4n_var2name
VAR2NAME_OBJS = rh4n_v2n.o \
			    rh4n_v2n_resolvenames.o \
			    rh4n_v2n_resolveposition.o

VAR2NAME_LIB = librh4nvar2name.a

var2name: vars logging ldaparser var2name_clean var2name_pre $(VAR2NAME_OBJS)
	@printf "Creating $(LIBOUTPUT)/$(VAR2NAME_LIB)\n"
	@$(AR) -cr $(LIBOUTPUT)/$(VAR2NAME_LIB) $(VAR2NAME_BIN)/*.o
	@printf "Done compiling and linking var2name\n"

$(VAR2NAME_OBJS):
	@printf "CC $(VAR2NAME_SRC)/$*.c => $(VAR2NAME_BIN)/$*.o\n"
	@$(CC) $(CARGS1) -o $(VAR2NAME_BIN)/$*.o $(VAR2NAME_SRC)/$*.c

var2name_pre:
	@printf "Creating var2name output folder\n"
	@mkdir -p $(VAR2NAME_BIN)
	@mkdir -p $(LIBOUTPUT)

var2name_clean:
	@printf "Cleaning var2name library\n"
	@rm -f $(LIBOUTPUT)/$(UTILS_BIN)
	@printf "Cleaning var2name objects\n"
	@rm -f $(VAR2NAME_BIN)/*.o

var2name_test: var2name
	@$(CC) -g ./libs/rh4n_var2name/test/main.c $(INCLUDE) -o ./libs/rh4n_var2name/test/main $(LIBS)
	@./libs/rh4n_var2name/test/main

#                         +-----------------+
#-------------------------|    Messaging    |----------------------------------
#                         +-----------------+

MESSAGING_SRC = ./libs/rh4n_messaging/src
MESSAGING_BIN = ./bin/rh4n_messaging
MESSAGING_OBJS = rh4n_messaging.o \
				 rh4n_messaging_uds.o \
				 rh4n_messaging_sessionInformations.o \
				 rh4n_messaging_sendvars.o \
				 rh4n_messaging_recvvars.o \
				 rh4n_messaging_textBlock.o
MESSAGING_LIB = librh4nmessaging.a

messaging: logging messaging_clean messaging_pre $(MESSAGING_OBJS)
	@printf "Creating $(LIBOUTPUT)/$(MESSAGING_LIB)\n"
	@$(AR) -cr $(LIBOUTPUT)/$(MESSAGING_LIB) $(MESSAGING_BIN)/*.o
	@printf "Done compiling and linking messaging\n"

$(MESSAGING_OBJS):
	@printf "CC $(MESSAGING_SRC)/$*.c => $(MESSAGING_BIN)/$*.o\n"
	@$(CC) $(CARGS1) -o $(MESSAGING_BIN)/$*.o $(MESSAGING_SRC)/$*.c

messaging_pre:
	@printf "Creating messaging output folder\n"
	@mkdir -p $(MESSAGING_BIN)
	@mkdir -p $(LIBOUTPUT)

messaging_clean:
	@printf "Creating messaging library\n"
	@rm -f $(LIBOUTPUT)/$(MESSAGING_LIB)
	@printf "Cleaning messaging objects\n"
	@rm -f $(MESSAGING_BIN)/*.o

messaging_tests: messaging tests_pre
	@printf "Building messaging tests\n"
	@printf "CC $(MESSAGING_SRC)/../tests/server.c => $(TESTOUTPUT)/messaging_server\n"
	@$(CC) -g $(MESSAGING_SRC)/../tests/server.c -o $(TESTOUTPUT)/messaging_server $(INCLUDE) $(LIBS)
	@printf "CC $(MESSAGING_SRC)/../tests/client.c => $(TESTOUTPUT)/messaging_client\n"
	@$(CC) -g $(MESSAGING_SRC)/../tests/client.c -o $(TESTOUTPUT)/messaging_client $(INCLUDE) $(LIBS)
	@printf "CC $(MESSAGING_SRC)/../tests/exec.c => $(TESTOUTPUT)/messaging_exec\n"
	@$(CC) -g $(MESSAGING_SRC)/../tests/exec.c -o $(TESTOUTPUT)/messaging_exec $(INCLUDE) $(LIBS)


#                         +-----------------+
#-------------------------|   Natuser lib   |----------------------------------
#                         +-----------------+

NATUSER_OBJS = rh4n_nat_jumptab.o \
			   ncuxinit.o \
			   rh4n_nat_genjson.o \
			   rh4n_nat_init.o \
			   rh4n_nat_utils.o \
			   rh4n_nat_variablereadout.o \
			   rh4n_nat_geturlobj.o \
			   rh4n_nat_mbn.o \
			   rh4n_nat_getbodyobj.o \
			   rh4n_nat_getbodyvar.o \
			   rh4n_nat_geturlvar.o \
			   rh4n_nat_getuser.o \
			   rh4n_nat_debug_init.o \
			   rh4n_nat_sleep.o
			   
NATUSER_READOUT_OBJS = rh4n_nat_a_handling.o \
					   rh4n_nat_i_handling.o \
					   rh4n_nat_l_handling.o \
					   rh4n_nat_f_handling.o \
					   rh4n_nat_dt_handling.o \
					   rh4n_nat_np_handling.o \
					   rh4n_nat_u_handling.o

NATUSER_PUTVAR_OBJS = rh4n_nat_put_a.o \
					  rh4n_nat_put_i.o \
					  rh4n_nat_put_l.o \
					  rh4n_nat_put_f.o \
					  rh4n_nat_put_aton.o

NATUSER_SRC = ./natuser/src
NATUSER_BIN = ./bin/natuserlib

natuserlib: utils logging vars json_generator ldaparser var2name natuserlib_clean natuserlib_pre \
			$(NATUSER_OBJS) $(NATUSER_READOUT_OBJS) $(NATUSER_PUTVAR_OBJS)
	@printf "Linking librealHTML4Natural.so\n"
	@$(CC) $(LFLAGS1_SO) $(NATUSER_BIN)/*.o $(LIBS) $(LFLAGS2_SO) -o ./bin/librealHTML4Natural.so
	@printf "Done compiling and linking natuserlib\n"

nat_test: natuserlib
	@printf "Cleaning Natural test\n"
	@rm -f ./natuser_lib/test/rh4n_nat_test_genjson
	@printf "CC rh4n_nat_test_genjson.c => rh4n_nat_test_genjson.c\n"
	@$(CC) -g ./natuser_lib/test/rh4n_nat_test_genjson.c $(INCLUDE) \
		-o ./natuser_lib/test/rh4n_nat_test_genjson $(LIBS)
	@printf "Running test\n"
	@export NATUSER=./bin/librealHTML4Natural.so; ./natuser_lib/test/rh4n_nat_test_genjson
	

$(NATUSER_OBJS): 
	@printf "CC $(NATUSER_SRC)/$*.c => $(NATUSER_BIN)/$*.o\n"
	@$(CC) $(CARGS_SO) -o $(NATUSER_BIN)/$*.o $(NATUSER_SRC)/$*.c

$(NATUSER_READOUT_OBJS): 
	@printf "CC $(NATUSER_SRC)/$*.c => $(NATUSER_BIN)/var_parm_handling/$*.o\n"
	@$(CC) $(CARGS_SO) -o $(NATUSER_BIN)/$*.o $(NATUSER_SRC)/var_parm_handling/$*.c

$(NATUSER_PUTVAR_OBJS):
	@printf "CC $(NATUSER_SRC)/put_var_handling/$*.c => $(NATUSER_BIN)/$*.o\n"
	@$(CC) $(CARGS_SO) -o $(NATUSER_BIN)/$*.o $(NATUSER_SRC)/put_var_handling/$*.c

natuserlib_pre:
	@printf "Creating natuser output folder\n"
	@mkdir -p $(NATUSER_BIN)

natuserlib_clean:
	@printf "Cleaning natuser objects\n"
	@rm -f $(NATUSER_BIN)/*.o
	@rm -f ./bin/librealHTML4Natural.so



#                         +-----------------+
#-------------------------|    natcaller    |----------------------------------
#                         +-----------------+

NATCALLER_OBJS = rh4n_natcaller_main.o \
				 rh4n_natcaller_parmgeneration.o \
				 rh4n_natcaller_natural.o \
				 rh4n_natcaller_errorhandling.o

NATCALLER_SRC = ./natcaller/src
NATCALLER_BIN = ./bin/natcaller

natcaller: utils logging vars messaging natcaller_clean natcaller_pre $(NATCALLER_OBJS)
	@printf "Linking Natcaller\n"
	@$(CC) -o ./bin/realHTML4NaturalNatCaller $(NATCALLER_BIN)/*.o $(LIBS)

$(NATCALLER_OBJS):
	@printf "CC $(NATCALLER_SRC)/$*.c => $(NATCALLER_BIN)/$*.o\n"
	@$(CC) $(CARGS_SO) -o $(NATCALLER_BIN)/$*.o $(NATCALLER_SRC)/$*.c

natcaller_clean:
	@printf "Cleaning natcaller objects\n"
	@rm -f $(NATCALLER_BIN)/*.o
	@rm -f ./bin/realHTML4NaturalNatCaller

natcaller_pre:
	@printf "Creating natcaller output folder\n"
	@mkdir -p $(NATCALLER_BIN)

