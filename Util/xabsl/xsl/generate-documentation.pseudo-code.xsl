<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL" xmlns:dotml="http://www.martin-loetzsch.de/DOTML">
	<xsl:key name="enumerated-input-symbols" match="//xabsl:enumerated-input-symbol" use="@name"/>
	<xsl:key name="decimal-input-symbols" match="//xabsl:decimal-input-symbol" use="@name"/>
	<xsl:key name="boolean-input-symbols" match="//xabsl:boolean-input-symbol" use="@name"/>
	<xsl:key name="constants" match="//xabsl:constant" use="@name"/>
	<xsl:output method="xml" indent="yes"/>
	<xsl:template name="pseudo-code">
		<xsl:param name="current-state"/>
		<xsl:param name="id-string"/>
		<table>
			<xsl:attribute name="class"><xsl:choose><xsl:when test="ancestor-or-self::xabsl:common-decision-tree">common-pseudo-code-frame</xsl:when><xsl:otherwise>pseudo-code-frame</xsl:otherwise></xsl:choose></xsl:attribute>
			<tr>
				<td>
					<table cellpadding="0" cellspacing="0">
						<xsl:choose>
							<xsl:when test="local-name(xabsl:*)='if'   or local-name(xabsl:*)='else'">
								<xsl:apply-templates>
									<xsl:with-param name="current-state" select="$current-state"/>
									<xsl:with-param name="id-string" select="$id-string"/>
								</xsl:apply-templates>
							</xsl:when>
							<xsl:otherwise>
								<tr>
									<xsl:for-each select="xabsl:*">
										<xsl:call-template name="apply-template-in-new-table-cell">
											<xsl:with-param name="current-state" select="$current-state"/>
										</xsl:call-template>
									</xsl:for-each>
								</tr>
							</xsl:otherwise>
						</xsl:choose>
					</table>
				</td>
			</tr>
		</table>
	</xsl:template>
	<xsl:template name="apply-template-in-new-table-cell">
		<xsl:param name="current-state"/>
		<xsl:choose>
			<xsl:when test="local-name()='not' or local-name()='and' or local-name()='or'">
				<td>
					<xsl:apply-templates select=".">
						<xsl:with-param name="current-state" select="$current-state"/>
					</xsl:apply-templates>
				</td>
			</xsl:when>
			<xsl:otherwise>
				<td class="pseudo-code-cell">
					<xsl:apply-templates select=".">
						<xsl:with-param name="current-state" select="$current-state"/>
					</xsl:apply-templates>
				</td>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="xabsl:if">
		<xsl:param name="current-state"/>
		<xsl:param name="id-string"/>
		<tr>
			<td>
				<table cellpadding="0" cellspacing="0">
					<tr>
						<td>
							<table cellpadding="0" cellspacing="0">
								<tr id="{$id-string}_{count(preceding-sibling::xabsl:if)}">
									<td class="pseudo-code-left-bracket" nowrap="nowrap">
										<span class="bold">
											<xsl:if test="local-name()='if'">if</xsl:if>
										</span>&#160;(</td>
									<xsl:for-each select="xabsl:condition">
										<xsl:for-each select="xabsl:*">
											<xsl:call-template name="apply-template-in-new-table-cell">
												<xsl:with-param name="current-state" select="$current-state"/>
											</xsl:call-template>
										</xsl:for-each>
										<td class="pseudo-code-right-bracket">)</td>
										<td class="pseudo-code-cell" align="right">
											<span class="italic">
												<span class="nowrap">&#160;&#160;&#160;// <xsl:value-of select="@description"/>
												</span>
											</span>
										</td>
									</xsl:for-each>
								</tr>
							</table>
						</td>
					</tr>
					<tr>
						<td>
							<table cellpadding="0" cellspacing="0">
								<tr>
									<td colspan="4" class="pseudo-code-cell">{</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr>
						<td colspan="4">
							<table cellpadding="0" cellspacing="0">
								<tr>
									<td class="pseudo-code-cell" nowrap="nowrap">&#160;&#160;</td>
									<td>
										<table cellpadding="0" cellspacing="0">
											<xsl:apply-templates>
												<xsl:with-param name="current-state" select="$current-state"/>
												<xsl:with-param name="id-string">
													<xsl:value-of select="$id-string"/>_<xsl:value-of select="count(preceding-sibling::xabsl:if)"/>
												</xsl:with-param>
											</xsl:apply-templates>
										</table>
									</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr>
						<td>
							<table cellpadding="0" cellspacing="0">
								<tr>
									<td colspan="4" class="pseudo-code-cell">}</td>
								</tr>
							</table>
						</td>
					</tr>
				</table>
			</td>
		</tr>
		<xsl:if test="ancestor-or-self::xabsl:common-decision-tree and count(following-sibling::xabsl:*)=0">
			<tr>
				<td>
					<table cellpadding="0" cellspacing="0">
						<tr>
							<td>
								<table cellpadding="0" cellspacing="0">
									<tr>
										<td colspan="4" class="pseudo-code-cell">
											<span class="bold">else</span>
										</td>
									</tr>
								</table>
							</td>
						</tr>
						<tr>
							<td>
								<table cellpadding="0" cellspacing="0">
									<tr>
										<td colspan="4" class="pseudo-code-cell">{</td>
									</tr>
								</table>
							</td>
						</tr>
						<tr>
							<td colspan="4">
								<table cellpadding="0" cellspacing="0">
									<tr>
										<td class="pseudo-code-cell" nowrap="nowrap">&#160;&#160;</td>
										<td>
											<xsl:for-each select="ancestor::xabsl:option/xabsl:state[@name=$current-state]/xabsl:decision-tree">
												<xsl:call-template name="pseudo-code">
													<xsl:with-param name="current-state" select="$current-state"/>
													<xsl:with-param name="id-string">
														<xsl:value-of select="$id-string"/>_<xsl:value-of select="1"/>
													</xsl:with-param>
												</xsl:call-template>
											</xsl:for-each>
										</td>
									</tr>
								</table>
							</td>
						</tr>
						<tr>
							<td>
								<table cellpadding="0" cellspacing="0">
									<tr>
										<td colspan="4" class="pseudo-code-cell">}</td>
									</tr>
								</table>
							</td>
						</tr>
					</table>
				</td>
			</tr>
		</xsl:if>
	</xsl:template>
	<xsl:template match="xabsl:else">
		<xsl:param name="current-state"/>
		<xsl:param name="id-string"/>
		<tr>
			<td>
				<table cellpadding="0" cellspacing="0">
					<tr>
						<td>
							<table cellpadding="0" cellspacing="0">
								<tr>
									<td colspan="4" class="pseudo-code-cell">
										<span class="bold">else</span>
									</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr>
						<td>
							<table cellpadding="0" cellspacing="0">
								<tr>
									<td colspan="4" class="pseudo-code-cell">{</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr>
						<td colspan="4">
							<table cellpadding="0" cellspacing="0">
								<tr>
									<td class="pseudo-code-cell" nowrap="nowrap">&#160;&#160;</td>
									<td>
										<table cellpadding="0" cellspacing="0">
											<xsl:apply-templates>
												<xsl:with-param name="current-state" select="$current-state"/>
												<xsl:with-param name="id-string">
													<xsl:value-of select="$id-string"/>_<xsl:value-of select="1"/>
												</xsl:with-param>
											</xsl:apply-templates>
										</table>
									</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr>
						<td>
							<table cellpadding="0" cellspacing="0">
								<tr>
									<td colspan="4" class="pseudo-code-cell">}</td>
								</tr>
							</table>
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</xsl:template>
	<xsl:template match="xabsl:transition-to-state">
		<xsl:param name="current-state"/>
		<tr>
		  <xsl:choose>
		  	<xsl:when test="not(ancestor-or-self::xabsl:common-decision-tree) and @ref=$current-state">
					<td class="pseudo-code-cell">stay;</td>
		  	</xsl:when>
		  	<xsl:otherwise>
					<td class="pseudo-code-cell">goto <a href="#state_{@ref}">
							<xsl:value-of select="@ref"/>
						</a>;
					</td>
				</xsl:otherwise>
			</xsl:choose>
		</tr>
	</xsl:template>
	<xsl:template match="xabsl:condition"/>
	<xsl:template match="xabsl:or | xabsl:and">
		<table cellpadding="0" cellspacing="0">
			<tr>
				<td>
					<table cellpadding="0" cellspacing="0">
						<tr>
							<td class="pseudo-code-left-bracket" nowrap="nowrap">(</td>
							<xsl:for-each select="xabsl:*[1]">
								<xsl:call-template name="apply-template-in-new-table-cell"/>
							</xsl:for-each>
							<td class="pseudo-code-right-bracket">)</td>
						</tr>
					</table>
				</td>
			</tr>
			<xsl:for-each select="xabsl:*[position()>1]">
				<tr>
					<td>
						<table cellpadding="0" cellspacing="0">
							<tr>
								<td nowrap="nowrap" class="pseudo-code-cell">
									<xsl:choose>
										<xsl:when test="local-name(..)='or'">||&#160;</xsl:when>
										<xsl:when test="local-name(..)='and'">&amp;&amp;&#160;</xsl:when>
									</xsl:choose>
								</td>
								<td>
									<table cellpadding="0" cellspacing="0">
										<tr>
											<td class="pseudo-code-left-bracket" nowrap="nowrap">(</td>
											<xsl:call-template name="apply-template-in-new-table-cell"/>
											<td valign="bottom" class="pseudo-code-right-bracket" nowrap="nowrap">)</td>
										</tr>
									</table>
								</td>
							</tr>
						</table>
					</td>
				</tr>
			</xsl:for-each>
		</table>
	</xsl:template>
	<xsl:template match="xabsl:not">
		<table cellpadding="0" cellspacing="0">
			<tr>
				<td class="pseudo-code-left-bracket" nowrap="nowrap">!(</td>
				<xsl:for-each select="xabsl:*[1]">
					<xsl:call-template name="apply-template-in-new-table-cell"/>
				</xsl:for-each>
				<td class="pseudo-code-right-bracket">)</td>
			</tr>
		</table>
	</xsl:template>
	<xsl:template match="xabsl:equal-to | xabsl:not-equal-to | xabsl:less-than | xabsl:less-than-or-equal-to | xabsl:greater-than | xabsl:greater-than-or-equal-to | xabsl:enumerated-input-symbol-comparison">
		<xsl:apply-templates select="xabsl:*[1]"/>
		<span class="nowrap">
			<xsl:choose>
				<xsl:when test="local-name()='equal-to'"> == </xsl:when>
				<xsl:when test="local-name()='not-equal-to'"> != </xsl:when>
				<xsl:when test="local-name()='less-than'"> &lt; </xsl:when>
				<xsl:when test="local-name()='less-than-or-equal-to'"> &lt; </xsl:when>
				<xsl:when test="local-name()='greater-than'"> > </xsl:when>
				<xsl:when test="local-name()='greater-than-or-equal-to'"> >= </xsl:when>
				<xsl:when test="local-name()='enumerated-input-symbol-comparison'"> == </xsl:when>
			</xsl:choose>
			<xsl:apply-templates select="xabsl:*[2]"/>
		</span>
	</xsl:template>
	<xsl:template match="xabsl:plus | xabsl:minus | xabsl:multiply | xabsl:divide | xabsl:mod">(<xsl:apply-templates select="xabsl:*[1]"/>
		<span class="nowrap">
			<xsl:choose>
				<xsl:when test="local-name()='plus'"> + </xsl:when>
				<xsl:when test="local-name()='minus'"> - </xsl:when>
				<xsl:when test="local-name()='multiply'"> * </xsl:when>
				<xsl:when test="local-name()='divide'"> / </xsl:when>
				<xsl:when test="local-name()='mod'"> % </xsl:when>
			</xsl:choose>
			<xsl:apply-templates select="xabsl:*[2]"/>)</span>
	</xsl:template>
	<xsl:template match="xabsl:decimal-input-symbol-ref">
		<a href="symbols.{key('decimal-input-symbols',@ref)/../@id}.html#{@ref}" title="{key('decimal-input-symbols',@ref)/@description}">
			<xsl:value-of select="@ref"/>
		</a>(<xsl:for-each select="xabsl:with-parameter">
			<span class="nowrap">
				<xsl:apply-templates/>
				<xsl:if test="count(following-sibling::xabsl:*)>0">, </xsl:if>
			</span>
		</xsl:for-each>)
	</xsl:template>
	<xsl:template match="xabsl:boolean-input-symbol-ref">
		<a href="symbols.{key('boolean-input-symbols',@ref)/../@id}.html#{@ref}" title="{key('boolean-input-symbols',@ref)/@description}">
			<xsl:value-of select="@ref"/>
		</a>(<xsl:for-each select="xabsl:with-parameter">
			<span class="nowrap">
				<xsl:apply-templates/>
				<xsl:if test="count(following-sibling::xabsl:*)>0">, </xsl:if>
			</span>
		</xsl:for-each>)
	</xsl:template>
	<xsl:template match="xabsl:enumerated-input-symbol-ref">
		<a href="symbols.{key('enumerated-input-symbols',@ref)/../@id}.html#{@ref}" title="{key('enumerated-input-symbols',@ref)/@description}">
			<xsl:value-of select="@ref"/>
		</a>(<xsl:for-each select="xabsl:with-parameter">
			<span class="nowrap">
				<xsl:apply-templates/>
				<xsl:if test="count(following-sibling::xabsl:*)>0">, </xsl:if>
			</span>
		</xsl:for-each>)
	</xsl:template>
	<xsl:template match="xabsl:constant-ref">
		<a href="symbols.{key('constants',@ref)/../@id}.html#{@ref}" title="{key('constants',@ref)/@description}  ({key('constants',@ref)/@value})">
			<xsl:value-of select="@ref"/>
		</a>
	</xsl:template>
	<xsl:template match="xabsl:conditional-expression">(<xsl:for-each select="xabsl:condition">
			<xsl:apply-templates/>
		</xsl:for-each>?<xsl:for-each select="xabsl:expression1">
			<xsl:apply-templates/>
		</xsl:for-each>:<xsl:for-each select="xabsl:expression2">
			<xsl:apply-templates/>
		</xsl:for-each>)</xsl:template>
	<xsl:template match="xabsl:option-parameter-ref">
		<xsl:variable name="option-name" select="ancestor::xabsl:option/@name"/>
		<xsl:variable name="ref" select="@ref"/>::<a href="option.{ancestor::xabsl:option/@name}.html" title="Option parameter {@ref}: {ancestor::xabsl:option/xabsl:option-definitions/xabsl:option-definition[@name=$option-name]/xabsl:parameter[@name=$ref]/@description}">
			<xsl:value-of select="@ref"/>
		</a>
	</xsl:template>
	<xsl:template match="xabsl:decimal-value">
		<xsl:value-of select="@value"/>
	</xsl:template>
	<xsl:template match="xabsl:boolean-value">
		<xsl:value-of select="@value"/>
	</xsl:template>
	<xsl:template match="xabsl:enum-element-ref">
		<xsl:value-of select="@ref"/>
	</xsl:template>
	<xsl:template match="xabsl:subsequent-option-reached-target-state">subsequent-option-reached-target-state</xsl:template>
	<xsl:template match="xabsl:*" priority="-10000">
		<xsl:value-of select="local-name()"/>
	</xsl:template>
</xsl:stylesheet>
