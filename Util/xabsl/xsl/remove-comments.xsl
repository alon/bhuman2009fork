<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL">
	<xsl:template match="*" priority="-1000">
		<xsl:copy>
			<xsl:copy-of select="./@*"/>
			<xsl:apply-templates/>
		</xsl:copy>
	</xsl:template>
	<xsl:template match="text()" priority="-1000">
		<xsl:value-of select="."/>
	</xsl:template>
	<xsl:template match="comment()">
		<!-- the aim of this stylesheet is to remove all comments from a xml file and to reproduce the rest of it. -->
	</xsl:template>
</xsl:stylesheet>
