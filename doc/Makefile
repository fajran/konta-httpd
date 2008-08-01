all : xhtml

html : xhtml

onehtml : onexhtml

xhtml :
	@cd html; \
		xsltproc --param chunk.section.depth 5 --param use.id.as.filename 1 /usr/share/docbook-xsl/xhtml/chunk.xsl ../manual.xml

onexhtml :
	@cd html; \
		xsltproc --param use.id.as.filename 1 /usr/share/docbook-xsl/xhtml/html.xsl manual.xml

