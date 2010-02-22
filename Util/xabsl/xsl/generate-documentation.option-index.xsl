<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL" xmlns:dotml="http://www.martin-loetzsch.de/DOTML">
	<xsl:import href="generate-documentation.menu.xsl"/>
	<xsl:output method="html" indent="yes"/>
	<xsl:template match="xabsl:option-definitions">
		<html>
			<head>
				<title>XABSL2 Behavior Documentation: Options</title>
				<link rel="stylesheet" type="text/css" href="styles.css">
					<xsl:text> </xsl:text>
				</link>
			</head>
			<body>
				<table border="0" cellpadding="0" cellspacing="0">
					<tr>
						<td class="menu-cell">
							<table  border="0" cellspacing="0" cellpadding="1">
								<xsl:call-template name="menu-xabsl2-logo"/>
								<xsl:call-template name="menu-entry-index-linked"/>
								<xsl:call-template name="menu-entry-agents-linked"/>
								<xsl:call-template name="menu-entry-symbols-linked"/>
								<xsl:call-template name="menu-entry-basic-behaviors-linked"/>
								<xsl:call-template name="menu-entry-options"/>
								<xsl:call-template name="menu-options"/>
							</table>
						</td>
						<td class="main-area">
							<h1>Options</h1>
							<table border="0" cellpadding="4" cellspacing="0">
								<xsl:for-each select="xabsl:option-definition">
									<tr>
										<td>
											<a href="option.{@name}.html" title="{@description}">
												<xsl:value-of select="@name"/>
											</a>
										</td>
										<td>
											<xsl:value-of select="@description"/>
										</td>
									</tr>
								</xsl:for-each>
							</table>
						</td>
					</tr>
				</table>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>
