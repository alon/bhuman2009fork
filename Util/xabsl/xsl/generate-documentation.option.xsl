<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL" xmlns:dotml="http://www.martin-loetzsch.de/DOTML">
	<xsl:import href="generate-documentation.option-tree.xsl"/>
	<xsl:import href="generate-documentation.parameters.xsl"/>
	<xsl:import href="generate-documentation.pseudo-code.xsl"/>
	<xsl:import href="generate-documentation.menu.xsl"/>
	<xsl:output method="xml" indent="yes"/>
	<xsl:param name="option-tree-xml"/>
	<xsl:key name="basic-behaviors" match="xabsl:option/xabsl:basic-behaviors/xabsl:basic-behavior" use="@name"/>
	<xsl:key name="option-definitions" match="xabsl:option/xabsl:option-definitions/xabsl:option-definition" use="@name"/>
	<xsl:key name="transitions" match="xabsl:option/descendant::xabsl:transition-to-state" use="@ref"/>
	<xsl:template match="xabsl:option">
		<xsl:variable name="name" select="@name"/>
		<xsl:variable name="description" select="xabsl:option-definitions/xabsl:option-definition[@name=$name]/@description"/>
		<html>
			<head>
				<title>XABSL2 Behavior Documentation: Option <xsl:value-of select="@name"/>
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
								<xsl:call-template name="menu-entry-options-linked"/>
								<xsl:call-template name="menu-option"/>
							</table>
						</td>
						<td class="main-area">
							<h1>Option <xsl:value-of select="@name"/>
							</h1>
							<p>
								<xsl:value-of select="$description"/>
							</p>
							<xsl:for-each select="key('option-definitions',@name)">
								<xsl:if test="xabsl:decimal-parameter | xabsl:boolean-parameter | xabsl:enumerated-parameter">
									<p>Parameters of that option:
								<xsl:call-template name="parameters"/>
									</p>
								</xsl:if>
							</xsl:for-each>
							<h2>State Machine</h2>
							<dotml:graph file-name="svg/option_{$name}" ranksep="0.4" nodesep="0.3">
								<dotml:cluster id="option" label="option {$name}" fontname="Arial" fontcolor="#005A9C" fontsize="12" fillcolor="#F4F4F4" style="filled">
									<xsl:call-template name="state-machine"/>
								</dotml:cluster>
								<dotml:cluster id="option_tree" color="#FFFFFF">
									<xsl:call-template name="paint-option-tree">
										<xsl:with-param name="root-option" select="@name"/>
										<xsl:with-param name="dont-paint-option" select="@name"/>
										<xsl:with-param name="option-tree-xml" select="$option-tree-xml"/>
									</xsl:call-template>
									<xsl:for-each select="xabsl:common-action/xabsl:subsequent-basic-behavior">
										<xsl:variable name="ref" select="@ref"/>
										<xsl:for-each select="../../xabsl:state">
											<dotml:edge arrowsize="0.8" color="#808080" style="dashed" minlen="3">
												<xsl:attribute name="from">state_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
												<xsl:attribute name="to">basic_behavior_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="$ref"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
											</dotml:edge>
										</xsl:for-each>
									</xsl:for-each>
									<xsl:for-each select="xabsl:common-action/xabsl:subsequent-option">
										<xsl:variable name="ref" select="@ref"/>
										<xsl:for-each select="../../xabsl:state">
											<dotml:edge minlen="2" arrowsize="0.8" color="#808080" style="dashed">
												<xsl:attribute name="from">state_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
												<xsl:attribute name="to">option_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="$ref"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
											</dotml:edge>
										</xsl:for-each>
									</xsl:for-each>
									<xsl:for-each select="xabsl:state/xabsl:subsequent-basic-behavior">
										<dotml:edge arrowsize="0.8" color="#808080" style="dashed" minlen="3">
											<xsl:attribute name="from">state_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="../@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
											<xsl:attribute name="to">basic_behavior_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@ref"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
										</dotml:edge>
									</xsl:for-each>
									<xsl:for-each select="xabsl:state/xabsl:subsequent-option">
										<dotml:edge minlen="2" arrowsize="0.8" color="#808080" style="dashed">
											<xsl:attribute name="from">state_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="../@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
											<xsl:attribute name="to">option_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@ref"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
										</dotml:edge>
									</xsl:for-each>
								</dotml:cluster>
							</dotml:graph>
							<xsl:apply-templates select="xabsl:state"/>
						</td>
					</tr>
				</table>
			</body>
		</html>
	</xsl:template>
	<xsl:template match="xabsl:state">
		<xsl:variable name="option-name" select="../@name"/>
		<xsl:variable name="current-state" select="@name"/>
		<h2 id="state_{@name}">State <xsl:value-of select="@name"/>
		</h2>
		<table cellpadding="0" cellspacing="5">
			<xsl:if test="@is-target-state='true'">
				<tr>
					<td colspan="5">This state is a target state.<br/>
						<br/>
					</td>
				</tr>
			</xsl:if>
			<tr>
				<td colspan="5">If that state is active, </td>
			</tr>
  		<xsl:for-each select="../xabsl:common-action/xabsl:subsequent-option | ../xabsl:common-action/xabsl:subsequent-basic-behavior | 
  		                      ../xabsl:common-action/xabsl:set-decimal-output-symbol | ../xabsl:common-action/xabsl:set-boolean-output-symbol | ../xabsl:common-action/xabsl:set-enumerated-output-symbol | 
  		                      xabsl:subsequent-option | xabsl:subsequent-basic-behavior | 
  		                      xabsl:set-decimal-output-symbol | xabsl:set-boolean-output-symbol | xabsl:set-enumerated-output-symbol">
				<xsl:choose>
  				<xsl:when test="name()='subsequent-option'">
						<tr>
							<td nowrap="nowrap">
								<span class="bold">&#160;>&#160;</span>
							</td>
							<td colspan="4">
								The option <a href="option.{@ref}.html" title="{key('option-definitions',@ref)/@description}">
											<xsl:value-of select="@ref"/>
										</a> is executed. <xsl:if test="xabsl:set-parameter">Parameters:</xsl:if>
							</td>
						</tr>
					</xsl:when>
		  		<xsl:when test="name()='subsequent-basic-behavior'">
						<tr>
							<td nowrap="nowrap">
								<span class="bold">&#160;>&#160;</span>
							</td>
							<td colspan="4">
								The <xsl:variable name="ref" select="@ref"/> basic behavior <a href="basic-behaviors.{key('basic-behaviors',$ref)/../@id}.html#{$ref}" title="{key('basic-behaviors',$ref)/@description}">
											<xsl:value-of select="@ref"/>
										</a> is executed. <xsl:if test="xabsl:set-parameter">Parameters:</xsl:if>
							</td>
						</tr>
		  	  </xsl:when>
		  		<xsl:when test="name()='set-decimal-output-symbol'">
						<tr>
							<td nowrap="nowrap">
								<span class="bold">&#160;>&#160;</span>
							</td>
	  					<td colspan="4">This decimal output symbol is set:</td>
	  				</tr>
	  				<tr>
  						<xsl:variable name="ref" select="@ref"/>
							<td nowrap="nowrap">&#160;</td>
							<td nowrap="nowrap">&#160;</td>
							<td class="pseudo-code-cell">
								<a href="symbols.{../../xabsl:symbols/xabsl:decimal-output-symbol[@name=$ref]/../@id}.html#symbols.{$ref}" title="{../../xabsl:symbols/xabsl:decimal-output-symbol[@name=$ref]/@description}">
									<xsl:value-of select="@ref"/>
								</a>
							</td>
							<td nowrap="nowrap" class="pseudo-code-cell">&#160;=&#160;</td>
							<td>
								<xsl:call-template name="pseudo-code"/>
							</td>
						</tr>
		  		</xsl:when>
		  		<xsl:when test="name()='set-boolean-output-symbol'">
						<tr>
							<td nowrap="nowrap">
								<span class="bold">&#160;>&#160;</span>
							</td>
	  					<td colspan="4">This boolean output symbol is set:</td>
	  				</tr>
	  				<tr>
  						<xsl:variable name="ref" select="@ref"/>
							<td>&#160;</td>
							<td>&#160;</td>
							<td class="pseudo-code-cell">
								<a href="symbols.{../../xabsl:symbols/xabsl:boolean-output-symbol[@name=$ref]/../@id}.html#symbols.{$ref}" title="{../../xabsl:symbols/xabsl:boolean-output-symbol[@name=$ref]/@description}">
									<xsl:value-of select="@ref"/>
								</a>
							</td>
							<td nowrap="nowrap" class="pseudo-code-cell">&#160;=&#160;</td>
							<td>
								<xsl:call-template name="pseudo-code"/>
							</td>
						</tr>
  				</xsl:when>
  				<xsl:when test="name()='set-enumerated-output-symbol'">
						<tr>
							<td nowrap="nowrap">
								<span class="bold">&#160;>&#160;</span>
							</td>
	  					<td colspan="4">This enumerated output symbol is set:</td>
	  				</tr>
	  				<tr>
  						<xsl:variable name="ref" select="@ref"/>
							<td nowrap="nowrap">&#160;</td>
							<td nowrap="nowrap">&#160;</td>
							<td class="pseudo-code-cell">
								<a href="symbols.{../../xabsl:symbols/xabsl:enumerated-output-symbol[@name=$ref]/../@id}.html#symbols.{$ref}" title="{../../xabsl:symbols/xabsl:enumerated-output-symbol[@name=$ref]/@description}">
									<xsl:value-of select="@ref"/>
								</a>
							</td>
							<td nowrap="nowrap" class="pseudo-code-cell">&#160;=&#160;</td>
						  <td class="pseudo-code-cell">
								<xsl:call-template name="pseudo-code"/>
							</td>
						</tr>
  				</xsl:when>
  			</xsl:choose>
				<xsl:if test="xabsl:set-parameter">
					<xsl:for-each select="xabsl:set-parameter">
						<xsl:variable name="ref" select="@ref"/>
						<xsl:variable name="subsequent" select="../@ref"/>
						<tr>
							<td>&#160;</td>
							<td>&#160;</td>
							<td class="pseudo-code-cell">
								<xsl:if test="../../xabsl:subsequent-option">
									<a href="option.{../@ref}.html" title="{key('option-definitions',$subsequent)/xabsl:parameter[@name=$ref]/@description}">
										<xsl:value-of select="@ref"/>
									</a>
								</xsl:if>
								<xsl:if test="../../xabsl:subsequent-basic-behavior">
									<a href="basic-behaviors.{key('basic-behaviors',$subsequent)/../@id}.html#{$subsequent}" title="{key('basic-behaviors',$subsequent)/xabsl:parameter[@name=$ref]/@description}">
										<xsl:value-of select="@ref"/>
									</a>
								</xsl:if>
							</td>
							<td nowrap="nowrap" class="pseudo-code-cell">&#160;=&#160;</td>
							<td class="pseudo-code-cell">
								<xsl:apply-templates/>;
							</td>
						</tr>
					</xsl:for-each>
				</xsl:if>
		  </xsl:for-each>
			<tr>
				<td colspan="5">&#160;</td>
			</tr>
			<tr>
				<td colspan="5">The decision tree:</td>
			</tr>
			<tr>
				<td>&#160;</td>
				<td colspan="4">
					<dotml:graph ranksep="0.4">
						<xsl:attribute name="file-name">svg/option_<xsl:value-of select="../@name"/>_state_<xsl:value-of select="@name"/></xsl:attribute>
						<dotml:cluster id="option" label="option {../@name}" fontname="Arial" fontcolor="#005A9C" fontsize="12" fillcolor="#F4F4F4" style="filled">
							<dotml:cluster id="states" style="filled" fillcolor="#F4F4F4" color="#F4F4F4">
								<xsl:for-each select=".//xabsl:transition-to-state | ../xabsl:common-decision-tree//xabsl:transition-to-state">
									<xsl:variable name="ref" select="@ref"/>
									<xsl:for-each select="ancestor::xabsl:option/xabsl:state[@name=$ref and @name!=$current-state]">
										<xsl:call-template name="paint-state-node">
											<xsl:with-param name="option-name" select="../@name"/>
										</xsl:call-template>
									</xsl:for-each>
									<xsl:for-each select="ancestor::xabsl:option/xabsl:state[@name=$ref and @name=$current-state]">
										<dotml:node label="{@name}" fontname="Arial" fontcolor="#005A9C" fontsize="9" style="dashed" fillcolor="#FFFFFF">
											<xsl:attribute name="id">state_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
											<xsl:attribute name="label"><xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">\n</xsl:with-param></xsl:call-template></xsl:attribute>
											<xsl:attribute name="shape"><xsl:choose><xsl:when test="../@initial-state=@name">Mcircle</xsl:when><xsl:when test="@is-target-state='true'">doublecircle</xsl:when><xsl:otherwise>circle</xsl:otherwise></xsl:choose></xsl:attribute>
										</dotml:node>
									</xsl:for-each>
								</xsl:for-each>
							</dotml:cluster>
							<dotml:cluster id="decision_tree" label="state {@name}" fillcolor="#FFFFFF" fontname="Arial" fontcolor="#005A9C" fontsize="12" style="filled">
								<dotml:node id="start" shape="point"/>
								<xsl:choose>
									<xsl:when test="../xabsl:common-decision-tree">
										<xsl:for-each select="../xabsl:common-decision-tree">
											<xsl:call-template name="decision-tree">
												<xsl:with-param name="parent">start</xsl:with-param>
												<xsl:with-param name="id-string">state_<xsl:value-of select="$current-state"/>_pseudo-code</xsl:with-param>
												<xsl:with-param name="current-state" select="$current-state"/>
											</xsl:call-template>
										</xsl:for-each>
									</xsl:when>
									<xsl:otherwise>
										<xsl:for-each select="xabsl:decision-tree">
											<xsl:call-template name="decision-tree">
												<xsl:with-param name="parent">start</xsl:with-param>
												<xsl:with-param name="id-string">state_<xsl:value-of select="$current-state"/>_pseudo-code</xsl:with-param>
												<xsl:with-param name="current-state" select="$current-state"/>
											</xsl:call-template>
										</xsl:for-each>
									</xsl:otherwise>
								</xsl:choose>
							</dotml:cluster>
						</dotml:cluster>
					</dotml:graph>
				</td>
			</tr>
			<tr>
				<td colspan="5">&#160;</td>
			</tr>
			<tr>
				<td colspan="5">Pseudo code of the decision tree:</td>
			</tr>
			<tr>
				<td>&#160;</td>
				<td colspan="4">
					<xsl:choose>
						<xsl:when test="../xabsl:common-decision-tree">
							<xsl:for-each select="../xabsl:common-decision-tree">
								<xsl:call-template name="pseudo-code">
									<xsl:with-param name="current-state" select="$current-state"/>
									<xsl:with-param name="id-string">state_<xsl:value-of select="$current-state"/>_pseudo-code</xsl:with-param>
								</xsl:call-template>
							</xsl:for-each>
						</xsl:when>
						<xsl:otherwise>
							<xsl:for-each select="xabsl:decision-tree">
								<xsl:call-template name="pseudo-code">
									<xsl:with-param name="current-state" select="$current-state"/>
									<xsl:with-param name="id-string">state_<xsl:value-of select="$current-state"/>_pseudo-code</xsl:with-param>
								</xsl:call-template>
							</xsl:for-each>
						</xsl:otherwise>
					</xsl:choose>
				</td>
			</tr>
		</table>
	</xsl:template>
	<xsl:template name="decision-tree">
		<xsl:param name="parent"/>
		<xsl:param name="id-string"/>
		<xsl:param name="current-state"/>
		<xsl:if test="xabsl:if">
			<dotml:record color="#FFFFFF" fontsize="9" height="0.15">
				<xsl:attribute name="fontname"><xsl:choose><xsl:when test="ancestor-or-self::xabsl:common-decision-tree">Arial Bold Italic</xsl:when><xsl:otherwise>Arial Bold</xsl:otherwise></xsl:choose></xsl:attribute>
				<dotml:node id="{$parent}_0" label="if"/>
  			<dotml:node id="{$parent}_1" label="else"/>
			</dotml:record>
			<dotml:edge from="{$parent}" to="{$parent}_0" dir="none">
				<xsl:attribute name="style"><xsl:choose><xsl:when test="ancestor-or-self::xabsl:common-decision-tree">dashed</xsl:when><xsl:otherwise>solid</xsl:otherwise></xsl:choose></xsl:attribute>
			</dotml:edge>
			<dotml:node id="{$parent}_0_" shape="box" fontsize="9" style="filled" fillcolor="#FFFFFF">
				<xsl:attribute name="fontname"><xsl:choose><xsl:when test="ancestor-or-self::xabsl:common-decision-tree">Arial Italic</xsl:when><xsl:otherwise>Arial</xsl:otherwise></xsl:choose></xsl:attribute>
				<xsl:attribute name="label"><xsl:call-template name="add-line-breaks"><xsl:with-param name="original-string"><xsl:value-of select="xabsl:if/xabsl:condition/@description"/></xsl:with-param><xsl:with-param name="min-length">5</xsl:with-param></xsl:call-template></xsl:attribute>
				<xsl:attribute name="URL">../option.<xsl:value-of select="ancestor::xabsl:option/@name"/>.html#<xsl:value-of select="$id-string"/>_0</xsl:attribute>
			</dotml:node>
			<dotml:edge from="{$parent}_0" to="{$parent}_0_" dir="none">
				<xsl:attribute name="style"><xsl:choose><xsl:when test="ancestor-or-self::xabsl:common-decision-tree">dashed</xsl:when><xsl:otherwise>solid</xsl:otherwise></xsl:choose></xsl:attribute>
			</dotml:edge>
			<xsl:for-each select="xabsl:if">
				<xsl:call-template name="decision-tree">
					<xsl:with-param name="parent">
						<xsl:value-of select="$parent"/>_0_</xsl:with-param>
					<xsl:with-param name="current-state" select="$current-state"/>
					<xsl:with-param name="id-string">
						<xsl:value-of select="$id-string"/>_0</xsl:with-param>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:if test="ancestor-or-self::xabsl:common-decision-tree and count(xabsl:else)=0">
				<xsl:for-each select="ancestor::xabsl:option/xabsl:state[@name=$current-state]/xabsl:decision-tree">
					<xsl:call-template name="decision-tree">
						<xsl:with-param name="parent">
							<xsl:value-of select="$parent"/>_1</xsl:with-param>
						<xsl:with-param name="current-state" select="$current-state"/>
						<xsl:with-param name="id-string">
							<xsl:value-of select="$id-string"/>_1</xsl:with-param>
					</xsl:call-template>
				</xsl:for-each>
			</xsl:if>
			<xsl:for-each select="xabsl:else">
				<xsl:call-template name="decision-tree">
					<xsl:with-param name="parent">
						<xsl:value-of select="$parent"/>_1</xsl:with-param>
					<xsl:with-param name="current-state" select="$current-state"/>
					<xsl:with-param name="id-string">
						<xsl:value-of select="$id-string"/>_1</xsl:with-param>
				</xsl:call-template>
			</xsl:for-each>
		</xsl:if>
		<xsl:if test="xabsl:transition-to-state">
			<dotml:edge from="{$parent}" minlen="2">
				<xsl:variable name="ref" select="xabsl:transition-to-state/@ref"/>
				<xsl:attribute name="style"><xsl:choose><xsl:when test="ancestor-or-self::xabsl:common-decision-tree">dashed</xsl:when><xsl:otherwise>solid</xsl:otherwise></xsl:choose></xsl:attribute>
				<xsl:attribute name="to">state_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="$ref"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
			</dotml:edge>
		</xsl:if>
	</xsl:template>
	<xsl:template name="paint-state-node">
		<xsl:param name="option-name"/>
		<dotml:node fontcolor="#005A9C" fontsize="9" style="filled" fillcolor="#FFFFFF" fontname="Arial" URL="../option.{$option-name}.html#state_{@name}">
			<xsl:attribute name="id">state_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
			<xsl:attribute name="label"><xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">\n</xsl:with-param></xsl:call-template></xsl:attribute>
			<xsl:attribute name="shape"><xsl:choose><xsl:when test="../@initial-state=@name">Mcircle</xsl:when><xsl:when test="@is-target-state='true'">doublecircle</xsl:when><xsl:otherwise>circle</xsl:otherwise></xsl:choose></xsl:attribute>
		</dotml:node>
	</xsl:template>
	<xsl:template name="state-machine">
		<xsl:variable name="name" select="@name"/>
		<dotml:sub-graph rank="same">
			<xsl:for-each select="xabsl:state[1]">
				<xsl:call-template name="paint-state-node">
					<xsl:with-param name="option-name" select="$name"/>
				</xsl:call-template>
			</xsl:for-each>
		</dotml:sub-graph>
		<xsl:for-each select="xabsl:state[position() mod 3 = 2]">
			<dotml:sub-graph rank="same">
				<xsl:call-template name="paint-state-node">
					<xsl:with-param name="option-name" select="$name"/>
				</xsl:call-template>
				<xsl:for-each select="following-sibling::xabsl:state[1]">
					<xsl:call-template name="paint-state-node">
						<xsl:with-param name="option-name" select="$name"/>
					</xsl:call-template>
				</xsl:for-each>
				<xsl:for-each select="following-sibling::xabsl:state[2]">
					<xsl:call-template name="paint-state-node">
						<xsl:with-param name="option-name" select="$name"/>
					</xsl:call-template>
				</xsl:for-each>
			</dotml:sub-graph>
		</xsl:for-each>
		<xsl:for-each select="xabsl:state">
			<xsl:variable name="name2" select="@name"/>
			<xsl:for-each select="descendant::xabsl:transition-to-state | ../xabsl:common-decision-tree/descendant::xabsl:transition-to-state">
				<xsl:if test="count(key('transitions',@ref)[ancestor::xabsl:state[@name=$name2] or ancestor::xabsl:common-decision-tree][1] | .) = 1">
					<dotml:edge>
						<xsl:attribute name="from">state_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="$name2"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
						<xsl:attribute name="to">state_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@ref"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
					</dotml:edge>
				</xsl:if>
			</xsl:for-each>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>
