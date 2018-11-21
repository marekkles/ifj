GRAPH_FILE=fsm

all: ${GRAPH_FILE}.svg 

${GRAPH_FILE}.svg: ${GRAPH_FILE}.gv
	dot -Tsvg $^ > $@
	