<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL">
	<xsl:template name="parameters">
		<xsl:if test="xabsl:decimal-parameter | xabsl:boolean-parameter | xabsl:enumerated-parameter">
			<table border="0" cellspacing="6" cellpadding="0">
				<tr>
					<td class="tablehead">Parameter</td>
					<td class="tablehead">Type</td>
					<td class="tablehead">Measure</td>
					<td class="tablehead">Range</td>
					<td class="tablehead">Description</td>
				</tr>
				<xsl:for-each select="xabsl:decimal-parameter">
					<tr>
						<td class="bold">
							<xsl:value-of select="@name"/>
						</td>
						<td>decimal</td>
						<td class="italic">
							<xsl:value-of select="@measure"/>
						</td>
						<td>
							<xsl:value-of select="@range"/>
						</td>
						<td>
							<xsl:value-of select="@description"/>
						</td>
					</tr>
				</xsl:for-each>
				<xsl:for-each select="xabsl:boolean-parameter">
					<tr>
						<td class="bold">
							<xsl:value-of select="@name"/>
						</td>
						<td>boolean</td>
			      <td class="italic">true/ false</td>
						<td/>
						<td>
							<xsl:value-of select="@description"/>
						</td>
					</tr>
				</xsl:for-each>
				<xsl:for-each select="xabsl:enumerated-parameter">
					<tr>
						<td class="bold">
							<xsl:value-of select="@name"/>
						</td>
						<td>enumerated</td>
						<td class="bold">
							<xsl:value-of select="@enumeration"/>
						</td>
						<td/>
						<td>
							<xsl:value-of select="@description"/>
						</td>
					</tr>
				</xsl:for-each>
			</table>
		</xsl:if>
	</xsl:template>
</xsl:stylesheet>
