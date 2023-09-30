# RealHTML4Natural

realHTML4Natural is a framework for the [Natural programming language](https://en.wikipedia.org/wiki/ADABAS#Natural_(4GL)) created by Software AG.

It contains the following features:

* generate JSON from Natural variable structures defined in local data areas
* match incoming JSON structures/variable names to Natural variable structures defined in local data areas
* get variables passed by the caller (usually from HTTP request in header and/or body)


The framework is split in two parts:

* Connectors
* Core

## Connectors

The connectors create the glue logic between the transport layer and Natural. Here the transport layer can be anything. A webserver, bash script, kafka connector, ...

Currently available connectors:

* TomcatConnector (exposes Natural via HTTP)

## Core

The core framework is responsible for calling natural and generating the actual JSON. It depends on the "interface4" functions provided by the Software AG to call Natural from the connector and User Exists from Natural. 

The advantage of the interface4 library compared to the "classic" user exists are that you get metdadata about the variables that are passed to the C-function like type, length, size and so on (Unfortunately the name and level is missing). In addition to that you can call subprograms and hand over parameter via a parameter data area.

### realHTML4NaturalNatCaller

Since some version of natural (round about 9.x.x) the program which calls a natural subprogram must be located in the Natural bin directory. Otherwise the natural runtime throws an error because some natural internal shared libraries seems to get loaded with a relative path to the current executed binary. This works normally because every Natural related binary is located in this bin directory. If you now have a program that calls Natural (via the interface4 calls) and is not located in the installation directory, these relative paths no longer fit accordingly and the reloading of the Natural internal libraries fails. 

The problem described above makes it necessary to separate the program that Natural finally calls from the connectors and to call it with the appropriate parameters.

```
bash-4.4$ /opt/softwareag/Natural/bin/realHTML4NaturalNatCaller
Usage: /opt/softwareag/Natural/bin/realHTML4NaturalNatCaller [-options] <propertiesFile>

options:
        -l <DEVELOP|DEBUG|INFO|WARN|ERROR|FATAL> default: ERROR
                set logging level
        -L <libname>
                set natural library which shows up in the logging
        -P <progname>
                set natural program which shows up in the logging
```

"propertiesFile" is a path to a file which is created by the function rh4nUtilsDumpProperties in the [utils lib](libs/rh4n_utils/src/rh4n_utils_prop.c) 

Normally it is not necessary to call this program manually. Rather then it get called by the glue logic in the used connector.

### librealHTML4Natural.so

Basically: This contains the API to get variables which are passed from the connector and generate JSON. For more information about these function, from an API perspective, see the [Usage](#Usage).

This shared library provides the user exists, which are described in the [Usage section](#Usage), a basic LDA parser, a library which can save variables in a structural way and a library to match these saved variables to a parsed LDA.

#### rh4n_utils
TODO: document

#### rh4n_logging
TODO: document

#### rh4n_vars
TODO: document

#### rh4n_ldaparser
TODO: document

#### rh4n_var2name
TODO: document

#### rh4n_jsongenerator
TODO: document

#### rh4n_messaging (Future use)
TODO: document

## Usage

### Parameter data area

In realHTML4Natural you only write subprograms as an entrypoint. These Subprograms need to have the following parameter data area defined. Otherwise you will get a NAT0969 Error (Parameter missmatch):

```
DEFINE DATA PARAMETER
/* >Natural Source Header 000000
/* :Mode S
/* :CP
/* <Natural Source Header
1 req-type (A) DYNAMIC
1 rh4n-internals (B) DYNAMIC
END-DEFINE
```

`req-type` indicates the request type and depends on the connector. For a HTTP connector it would be for example be "GET", "POST", "PUT" or "DELETE" and so on.

`rh4n-internals` is a pointer to a structure on the heap which get used by all user exists. You are not supposed to mess with this variable in Natural other then to pass is further.

### User exits

The following examples will refer to the following LDA with the name "LSHOP.NSL" in the library "DEMO":

```
DEFINE DATA LOCAL
/* >Natural Source Header 000000
/* :Mode S
/* :CP
/* <Natural Source Header

1 order-list (1:*)
    2 order-id (i4)
    2 customer-id (i4)
    2 article-count (i4)

1 order
    2 order-id (i4)
    2 customer-id (i4)
    2 customer-name (A) DYNAMIC
    2 article (1:*)
        3 article-id (I4)
        3 article-name (A) DYNAMIC
        3 article-quantity  (I4)

1 article-list (1:*)
    2 article-id (I4)
    2 article-name (A) DYNAMIC

1 article-details
    2 article-id (I4)
    2 article-name (A) DYNAMIC
    2 article-description (A) DYNAMIC
    2 price (N4.2)
END-DEFINE
```

#### Formatstring

In the description of the user exists, it is often spoken of a "formatting string". These serve to describe which structure is to be used in which LDA and have the following structure:

```
<structure name>@(<library>.)<LDA-Name>
```

* "<stucture name\>" is the name of the level 1 structure to use (case sensitive)
* "<library\>" can be omitted when the LDA is located in the same library as the called subprogram
* "<LDA-name\>" is the actual name of the LDA member without the ".NSL" extension

##### Examples

(Assumed that the subprogram lies in the library "DEMO")

* `order-list@LSHOP` would select the structure "order-list" from LDA "LSHOP.NSL" in the library "DEMO"
* `order-list@SYSTEM.LSHOP` would select the structure "order-list" from LDA "LSHOP.NSL" in the library "SYSTEM"
* `order-LIST@LSHOP` would select nothing because the structure name is case sensitive


#### RHGENJS (generate JSON)

#### Usage
```
call interface4 'RHGENJS' rh4n-internals <formatstring> <top-level-structure>
```

#### Examples

```
call interface4 'RHGENJS' rh4n-internals 'article-details@LSHOP' article-details
```
results in:
```
{
  "article-id": 4711,
  "article-name": "Article 4711",
  "article-description": "I am just a description",
  "price": "73.63"
}
```

---

```
call interface4 'RHGENJS' rh4n-internals 'order@LSHOP' order
```
results in:
```
{
  "order-id": 936,
  "customer-id": 376,
  "customer-name": "Customer 376",
  "article": [
    {
      "article-id": 4711,
      "article-name": "Article 4711",
      "article-quantity": 2
    },
    {
      "article-id": 815,
      "article-name": "Article 0815",
      "article-quantity": 73
    }
  ]
}
```

---

```
call interface4 'RHGENJS' rh4n-internals 'article-list@LSHOP' article-list(*)
```
results in:
```
[
  {
    "article-id": 4711,
    "article-name": "Article 4711"
  },
  {
    "article-id": 815,
    "article-name": "Article 0815"
  }
]
```


#### RHGETUO (get URL object)

Usage:
```
call interface4 'RHGETUO' rh4n-internals <formatstring> <top-level-structure>
```

#### RHGETUV (get URL variable)

Usage:
```
call interface4 'RHGETUV' rh4n-internals <name of variable> <natural variable>
```

#### RHGETBO (get body object)

Usage:
```
call interface4 'RHGETBO' rh4n-internals <formatstring> <top-level-structure>
```

#### RHGETBV (get body variable)

Usage:
```
call interface4 'RHGETBV' rh4n-internals <path to variable in tree structure> <natural variable>
```

#### RHGETUS (get user)

Usage:
```
call interface4 'RHGETUS' rh4n-internals <natural variable>
```

#### RHDBGINI (realHTML debug init)

Usage:
```
call interface4 'RHDBGINI' rh4n-internals <log level> <path to fuser> <path to outputfile>
```

#### RHSLEEP (sleep)

Usage:
```
call 'RHSLEEP'
```

just sleeps for one second

#### RHSETRC (set return code)
Future use. Will be used to indicate a return code to the calling connector
