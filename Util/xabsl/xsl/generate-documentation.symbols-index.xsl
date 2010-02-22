<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL" xmlns:dotml="http://www.martin-loetzsch.de/DOTML">
	<xsl:import href="generate-documentation.menu.xsl"/>
	<xsl:output method="html" indent="yes"/>
	<xsl:key name="symbols" match="xabsl:agent-collection/xabsl:options/xabsl:option/xabsl:symbols" use="@id"/>
	<xsl:template match="xabsl:agent-collection">
		<html>
			<head>
				<title>XABSL2 Behavior Documentation: Symbols</title>
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
								<xsl:call-template name="menu-entry-symbols"/>
								<xsl:call-template name="menu-symbols"/>
								<xsl:call-template name="menu-entry-basic-behaviors-linked"/>
								<xsl:call-template name="menu-entry-options-linked"/>
							</table>
						</td>
						<td class="main-area">
							<h1>Symbols</h1>
							<table border="0" cellpadding="4" cellspacing="0">
								<xsl:for-each select="xabsl:options/xabsl:option/xabsl:symbols[count(key('symbols',@id)[1] | .)=1]">
									<tr>
										<td class="bold">
											<div class="nowrap">
												<a href="symbols.{@id}.html" title="{@description}">
													<xsl:value-of select="@title"/>
												</a>
											</div>
										</td>
										<td>
											<xsl:value-of select="@description"/>
											<br/>
											<br/>
											<div class="menu-item">
												<xsl:for-each select="xabsl:*">
													<a href="symbols.{../@id}.html#{@name}" title="{@description}">
														<span class="nowrap">
															<xsl:value-of select="@name"/>
														</span>
													</a>&#160;&#160;&#160; </xsl:for-each>
											</div>
										</td>
									</tr>
									<tr>
										<td>&#160;</td>
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
