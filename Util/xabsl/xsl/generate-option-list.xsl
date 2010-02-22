<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL">
<xsl:template match="xabsl:agent-collection">
<xsl:for-each select="xabsl:options/xabsl:option">
<xsl:value-of select="@name"/>.xml
</xsl:for-each>

<xsl:for-each select="xabsl:options/xabsl:option/xabsl:symbols">
<xsl:value-of select="@id"/>.xml
</xsl:for-each>
</xsl:template>
</xsl:stylesheet>