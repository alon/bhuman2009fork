<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL">
	<xsl:import href="generate-documentation.parameters.xsl"/>
	<xsl:import href="generate-documentation.menu.xsl"/>
	<xsl:output method="html" indent="yes"/>
	<xsl:template match="xabsl:basic-behaviors">
		<html>
			<head>
				<title>XABSL2 Behavior Documentation: <xsl:value-of select="@title"/>
				</title>
				<link rel="stylesheet" type="text/css" href="styles.css">
					<xsl:text> </xsl:text>
				</link>
			</head>
			<body>
				<table border="0" cellpadding="0" cellspacing="0">
					<tr>
						<td class="menu-cell">
							<table border="0" cellspacing="0" cellpadding="1">
								<xsl:call-template name="menu-xabsl2-logo"/>
								<xsl:call-template name="menu-entry-index-linked"/>
								<xsl:call-template name="menu-entry-agents-linked"/>
								<xsl:call-template name="menu-entry-symbols-linked"/>
								<xsl:call-template name="menu-entry-basic-behaviors-linked"/>
								<xsl:call-template name="menu-basic-behavior"/>
								<xsl:call-template name="menu-entry-options-linked"/>
							</table>
						</td>
						<td class="main-area">
							<h1>
								<xsl:value-of select="@title"/>
							</h1>
							<p>
								<xsl:value-of select="@description"/>
							</p>
							<xsl:for-each select="xabsl:basic-behavior">
								<h2 id="{@name}">Basic Behavior &quot;<xsl:value-of select="@name"/>&quot;</h2>
								<p>
									<xsl:value-of select="@description"/>
								</p>
								<xsl:call-template name="parameters"/>
							</xsl:for-each>
						</td>
					</tr>
				</table>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>
