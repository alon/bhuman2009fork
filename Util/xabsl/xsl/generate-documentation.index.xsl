<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL">
	<xsl:import href="generate-documentation.menu.xsl"/>
	<xsl:output method="html" indent="yes"/>
	<xsl:template match="xabsl:agent-collection">
		<html>
			<head>
				<title>XABSL2 Behavior Documentation: <xsl:value-of select="xabsl:title"/>
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
								<xsl:call-template name="menu-entry-index"/>
								<xsl:call-template name="menu-entry-agents-linked"/>
								<xsl:call-template name="menu-entry-symbols-linked"/>
								<xsl:call-template name="menu-entry-basic-behaviors-linked"/>
								<xsl:call-template name="menu-entry-options-linked"/>
							</table>
						</td>
						<td class="main-area">
							<h1>
								<xsl:value-of select="xabsl:title"/>: Behavior Documentation</h1>
							<p>
								<div class="bold">Platform:</div>
								<xsl:value-of select="xabsl:platform"/>
							</p>
							<p>
								<div class="bold">Software Environment:</div>
								<xsl:value-of select="xabsl:software-environment"/>
							</p>
							<h2>Table Of Contents</h2>
							<table border="0" cellpadding="4" cellspacing="6">
								<tr>
									<td>
										<a href="agents.html" title="Available agents">Agents</a>
									</td>
									<td>All agents of <xsl:value-of select="xabsl:title"/>
									</td>
								</tr>
								<tr>
									<td>
										<a href="symbols.html" title="Available symbols">Symbols</a>
									</td>
									<td>The symbols needed to run the behavior in the software environment</td>
								</tr>
								<tr>
									<td>
										<a href="basic-behaviors.html" title="Available basic behaviors">Basic Behaviors</a>
									</td>
									<td>C++ written behaviors at the leaves of the option tree.</td>
								</tr>
								<tr>
									<td>
										<a href="options.html" title="Available options">Options</a>
									</td>
									<td>All options of all agents.</td>
								</tr>
							</table>
						</td>
					</tr>
				</table>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>
