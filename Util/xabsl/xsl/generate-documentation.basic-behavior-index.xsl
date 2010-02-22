<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL" xmlns:dotml="http://www.martin-loetzsch.de/DOTML">
	<xsl:import href="generate-documentation.menu.xsl"/>
	<xsl:output method="html" indent="yes"/>
	<xsl:key name="basic-behaviors" match="xabsl:agent-collection/xabsl:options/xabsl:option/xabsl:basic-behaviors" use="@id"/>
	<xsl:template match="xabsl:agent-collection">
		<html>
			<head>
				<title>XABSL2 Behavior Documentation: Basic Behaviors</title>
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
								<xsl:call-template name="menu-entry-basic-behaviors"/>
								<xsl:call-template name="menu-basic-behaviors"/>
								<xsl:call-template name="menu-entry-options-linked"/>
							</table>
						</td>
						<td class="main-area">
							<h1>Basic Behaviors</h1>
							<table border="0" cellpadding="4" cellspacing="0">
								<xsl:for-each select="xabsl:options/xabsl:option/xabsl:basic-behaviors[count(key('basic-behaviors',@id)[1] | .)=1]">
									<tr>
										<td colspan="2">
											<p>&#160;</p>
											<a href="basic-behaviors.{@id}.html" title="{@description}">
												<h3>
													<xsl:value-of select="@title"/>
												</h3>
											</a>
											<p>
												<xsl:value-of select="@description"/>
											</p>
											<p/>
										</td>
									</tr>
									<xsl:for-each select="xabsl:basic-behavior">
										<tr>
											<td>
												<a href="basic-behaviors.{../@id}.html#{@name}" title="{@description}">
													<xsl:value-of select="@name"/>
												</a>
											</td>
											<td>
												<xsl:value-of select="@description"/>
											</td>
										</tr>
									</xsl:for-each>
								</xsl:for-each>
							</table>
						</td>
					</tr>
				</table>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>
