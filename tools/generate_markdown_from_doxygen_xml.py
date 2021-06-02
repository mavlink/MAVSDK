#!/usr/bin/env python3

"""
This script converts Doxygen XML output into gitbook-compatible markdown.

- The directories that are parsed for source/outputs are passed using the input_dir argument.
- All files will be parsed, but only xml files for public c++ classes and struct are handled (implementation classes ignored).

How it works:
- There are Python objects for each C++ type - e.g. cppClass, cppTypdef etc. (cppClass covers both class and struct)
- Each object has a method import_from_doxygen_class_file() which takes a Tag (or filename for CPP) and parses in data.
  - The cpp class takes a file, and creates the sub objects it owns (e.g. methods), calling their import_from_doxygen_class_file() method with a tag.
- Each object (mostly) has a markdown_overview() and markdown() method that renders the markdown from the respective objects
- markdown_any_tag() -
  - Some objects/values will be empty and others will store tagged information that is only partially parsed from XML (in theory this allows us
  to change the output rendering, because we store it in generic format). The markdown_any_tag() method can be used to get
  markdown text or an empty string from any stored attribute.
  - This can be safely called on any "data" type attributed
  - This will report an error on console if an unsupported tag is detected. The tag will render in output anyway, but may contain odd tags.
- cleanup_markdown_string
  - Just cleans up a little of the markdown.

"""

import argparse
import os
import xml.etree.ElementTree as ET




def cleanup_markdown_string(aString):
    """
    Cleans up markdown so it looks a bit prettier
    - removes lots of empty lines
    - removes empty lines with lots of spaces.
    """
    #print('debug:cleanup_markdown_string')
    import re

    # Remove spaces following new line.
    def strip_spaces_empty_lines(matchobj):
        #print('debug:strip_spaces_empty_lines:group0: %s' % matchobj.group(0))
        return '\n'
    output_string = re.sub(r'^\s+$', strip_spaces_empty_lines, aString,flags=re.M)

    # Remove more than 2 empty lines.
    def strip_multiple_empty_lines(matchobj):
        print('debug:strip_multiple_empty_lines:group0: %s' % matchobj.group(0))
        return '\n\n'
    output_string = re.sub(r'\n{4,10}', strip_multiple_empty_lines, output_string)


    return output_string


def markdown_any_tag(aTag, html=False,para=True,consume=False):
    """
    Get markdown for any tag, with bugs reported for types that not specifically supported (and crap displayed in markdown). Intended for descriptions.
    aTag - the ElementTree tag or a string (returns string immediately)
    html - render the tag as HTML (False by default). Setting propagates to sub tags
    para - Turn off para tags - useful when a para is in a list.
    consume - return empty string. Removes everything below a particular point. A good way of hiding sub elements of a tree we know have been parsed - e.g. the simplesect tag in the detaileddescription tag contains return values, that we have pre-processed.
    """
    if consume:
        return '' #
    if aTag is None:
        return '' #Empty tag for whatever reason, just return empty string.
    if type(aTag)==str:
        return aTag #if the tag has already been converted "magically" you can just return the string.
    #print('debug:tag: %s' % aTag.tag)
    #print('debug:start:markdown_any_tag - aTag: %s' % str(ET.tostring(aTag),'utf-8'))
    #print('debug:tag: %s' % aTag.tag)
    #print('debug:html: %s, para: %s, consume: %s ' % (html, para,consume))
    lead_text=''
    child_text=''
    tail_text=''

    lead_text=aTag.text
    if not lead_text: #unless it is an empty tag!
       lead_text=''
    #print('lead_text: %s' % lead_text)
    #Next bit only gets run if there are children

    # Tail text test
    tail_text=aTag.tail
    if not tail_text:
        tail_text =''
    #print('tail_text: %s' % tail_text)

    #This section is for when you want to affect a sub tag based on a parent tag.
    #Main use is to prevent children of a particular type being rendered, when child processing handled elsewere.
    ignore_current_tag=False #Use to disable the current tag. Currently only used for turning off "computeroutput" if children are ref.
    for child in aTag:
        if child.tag=='simplesect':
            if child.attrib['kind']=='return':
                pass # simplesect contains return info for function. We parse it specifically in other code so here we make sure it this tag doesn't get processed.
            elif child.attrib['kind']=='see':
                pass #Prevent further handling of "see" children.
            elif child.attrib['kind']=='warning' or child.attrib['kind']=='note' or child.attrib['kind']=='since' or child.attrib['kind']=='attention' or child.attrib['kind']=='remark': #Pass 'note' types to specific handling
                child_text += markdown_any_tag(child,html=html,para=para) #.strip() //Handle warnings individually
            else:
                #Just in case there is a different type of simplesect, this tells us we need to think about it.
                print('Unhanded kind in simplesect tag in markdown_any_tag:kind: %s' % child.attrib['kind'])
                child_text+=str(ET.tostring(child),'utf-8')
        elif child.tag=='parameterlist':
            if child.attrib['kind']=='param':
                pass # We parse parameter info in the function param object code. Prevent further handling here.
            else:
                print('Unhanded kind in parameterlist tag in markdown_any_tag:kind: %s' % child.attrib['kind'])
                child_text+=str(ET.tostring(child),'utf-8')
        elif child.tag=='ref':
            if aTag.tag=='computeroutput':
                ignore_current_tag=True #Ignore computer output markup in links
            child_text += markdown_any_tag(child,html=html,para=para)
        elif child.tag=='computeroutput' or child.tag=='para' or child.tag=='listitem' or child.tag=='ulink' or child.tag=='bold' or child.tag=='emphasis' or child.tag=='verbatim':
            child_text += markdown_any_tag(child,html=html,para=para) #.strip()
        elif child.tag=='itemizedlist':
            child_text += markdown_any_tag(child,html=True,para=para)
        else:
            print('Unhanded tag in markdown_any_tag: %s' % child.tag)
            child_text+=str(ET.tostring(child),'utf-8')

    # This section handles processing of the current tag.
    # Current params like 'html' and ignore_current_tag can affect rendering.
    tag_text=''
    if aTag.tag=='computeroutput':
        if ignore_current_tag: #This tag is ignored, meaning that we don't add any special markup for it.
            tag_text=lead_text+child_text+tail_text
        else:
            if html:
                tag_text='<code>'+lead_text+child_text+'</code>'+tail_text
            else:
                tag_text='`'+lead_text+child_text+'`'+tail_text
    elif aTag.tag=='bold':
        if ignore_current_tag: #This tag is ignored, meaning that we don't add any special markup for it.
            tag_text=lead_text+child_text+tail_text
        else:
            if html:
                tag_text='<b>'+lead_text+child_text+'</b>'+tail_text
            else:
                tag_text='**'+lead_text+child_text+'**'+tail_text
    elif aTag.tag=='emphasis':
        if ignore_current_tag: #This tag is ignored, meaning that we don't add any special markup for it.
            tag_text=lead_text+child_text+tail_text
        else:
            if html:
                tag_text='<i>'+lead_text+child_text+'</i>'+tail_text
            else:
                tag_text='*'+lead_text+child_text+'*'+tail_text
    elif aTag.tag=='para':
        if para:
            if html:
                tag_text='<p>'+lead_text+child_text+'</p>'+tail_text
            else:
                tag_text='\n\n'+lead_text+child_text+'\n\n'+tail_text
        else: #para disabled, render without them.
            tag_text=lead_text+child_text+tail_text
    elif aTag.tag=='itemizedlist':
        if html:
            tag_text='\n<ul>\n'+lead_text.strip()+child_text.strip()+'\n</ul>'+tail_text
        else:
            tag_text='\n'+lead_text+child_text+tail_text+'\n' #single level implementation.
    elif aTag.tag=='listitem':
        if html:
            tag_text='\n<li>'+lead_text+child_text+'</li>'+tail_text
        else:
            tag_text='\n* '+lead_text+child_text+tail_text #single level implementation.
    elif aTag.tag=='ref':
        #Internal link tag handling.
        kind_ref=aTag.attrib['kindref']
        #print('kind_ref: %s' % kind_ref)
        ref_id=aTag.attrib['refid']
        #print('ref_id: %s' % ref_id)
        link_text=lead_text+child_text
        if kind_ref=='member':
            link_filename=ref_id.rsplit('_',1)[0]
            link_url='%s.md#%s' % (link_filename,ref_id)
        elif kind_ref=='compound':
            link_url='%s.md' % ref_id
        else:
           print('ERROR: Unsupported ref kind_ref: %s' % kind_ref)
           #exit()
        lead_text='[%s](%s)' % (link_text,link_url)
        #print('lead_text: %s' % lead_text)
        if html:
            lead_text='<a href="%s">%s</a>' % (link_url,link_text)
        tag_text=lead_text+tail_text #lead text includes child text (added above).
        #print ("ref tag_text: %s" % tag_text)

    elif aTag.tag=='ulink':
        #External link tag handling.
        link_url=aTag.attrib['url']
        #print('link_url: %s' % link_url)
        link_text=lead_text+child_text
        lead_text='[%s](%s)' % (link_text,link_url)
        #print('lead_text: %s' % lead_text)
        if html:
            lead_text='<a href="%s">%s</a>' % (link_url,link_text)
        tag_text=lead_text+tail_text #note, child text included in the URL text above. There won't be any though.
        #print('tag_text: %s' % tag_text)


    elif aTag.tag=='detaileddescription':
        tag_text=lead_text+child_text+tail_text


    elif aTag.tag=='simplesect':
        if aTag.attrib['kind']=='see' or aTag.attrib['kind']=='return':
            #Handle @note (note) and @sa (see) tags.
            # Note we should ONLY see this via the detaileddescription handling
            # Because children that are simplesect are not parsed.
            tag_text=lead_text+child_text+tail_text

        #Convert "note" types
        if aTag.attrib['kind']=='warning' or aTag.attrib['kind']=='note' or aTag.attrib['kind']=='attention':
            #print('Debug: kind %s' % aTag.attrib['kind'])
            noteTypeText=aTag.attrib['kind'].capitalize()
            if para:
                 if html:
                     tag_text='<p>'+lead_text+'<b>'+noteTypeText+': </b> '+child_text+'</p>'+tail_text
                 else: # ONLY THIS PATH TESTED (others should not occur, but leaving in as standard.
                     tag_text='\n\n> **'+noteTypeText+'** '+lead_text.strip()+child_text.strip()+'\n\n'+tail_text.strip()
            else: #para disabled, render without them.
                tag_text='\n\n> **'+noteTypeText+'** '+ lead_text.strip()+child_text.strip()+tail_text.strip()


    elif aTag.tag=='verbatim':
        tag_text='\n\n'+lead_text+child_text+'\n\n'+tail_text


    else:
        tag_text=lead_text+child_text+tail_text

    #print('debug:end:markdown_any_tag (%s) tag_text: %s' % (aTag.tag,tag_text.strip()))
    return tag_text


def seealso_from_tag(aTag):
    """
    Get see also info below any tag. Intended to be run on a detailed_description.
    """
    #Strip out seealso tags from detailed description (so it can be agreggated under Seealso section)
    #print('debug:function:seealso_from_tag: tag (%s)' % aTag.tag)
    seealso_tags=aTag.findall(".//simplesect[@kind='see']")
    seealso_text=''
    if seealso_tags:
        for item in seealso_tags:
            seealso_text+='\n- %s' % markdown_any_tag(item).strip()
        seealso_text='\n\n**See Also:**%s\n\n' % seealso_text
        #print('debug:seealso_from_tag: seealso_text (%s)' % seealso_text)
    return seealso_text



class cppAttribute:
    def __init__(self):
        self.kind='UNKNOWN'
        self.name = 'UNKNOWN'
        self.id = 'UNKNOWN'
        self.prot= 'UNKNOWN'
        self.static = 'UNKNOWN'
        self.mutable = 'UNKNOWN'

        self.type='UNKNOWN'
        self.definition='UNKNOWN'
        self.argsstring='UNKNOWN'
        self.initializer=''
        self.briefdescription='UNKNOWN'
        self.detaileddescription='UNKNOWN'
        self.inbodydescription='UNKNOWN'
        self.seealso = ''

    def import_from_doxygen_class_file(self, aTag):
        #print(aTag.attrib)
        self.name = aTag.find('name').text
        #print('attrib: self.name: %s' % self.name)
        self.id = aTag.attrib['id']
        self.prot= aTag.attrib['prot']
        self.static = aTag.attrib['static']
        self.kind = aTag.attrib['kind']
        self.mutable = aTag.attrib['mutable']

        self.type = aTag.find('type')
        self.definition = aTag.find('definition')
        self.argsstring = aTag.find('argsstring')
        self.initializer = aTag.find('initializer') # Needs more processing
        self.briefdescription = aTag.find('briefdescription')  # Needs more processing
        self.detaileddescription = aTag.find('detaileddescription') # Needs more processing
        self.seealso = seealso_from_tag(self.detaileddescription)
        self.inbodydescription = aTag.find('inbodydescription') # Needs more processing

    def markdown_overview(self):
        #print('debug:start:cppAttribute:markdown_overview')
        static_string=''
        if self.static=='yes':
            static_string='static '
        output_string=''
        #print('debug: static_string: %s' % static_string)
        #print('debug: self.type: %s' % self.type)
        #print('debug: self.name: %s' % self.name)
        #print('debug: self.initializer: %s' % self.initializer)
        #print('debug: self.briefdescription: %s' % markdown_any_tag(self.briefdescription).strip() )

        #Clean initialiser (removed inner spaces)
        cleaned_initializer=markdown_any_tag(self.initializer).strip()
        cleaned_initializer=" ".join(cleaned_initializer.split())

        output_string+='\n\n%s%s [%s](#%s) %s - %s\n\n' % (static_string,markdown_any_tag(self.type).strip(), self.name.strip(),self.id.strip(),cleaned_initializer,markdown_any_tag(self.briefdescription).strip() )
        return output_string

    def markdown(self):
        output_string=''
        output_string+='\n\n### %s {#%s}\n' % (self.name,self.id)

        #Get value of initialiser with no tags (won't work in code block)
        value_initializer =''
        temp_initializer = markdown_any_tag(self.initializer).strip()
        if not temp_initializer:
            pass #Empty initializer
            self.initializer=''
        else:
            if len(self.initializer):
                # We have tags in the initialiser. Strip them.
                value_initializer = self.initializer.find('.//ref').text #Get just the link text.
                if value_initializer: #initialiser contains links, but we've just pulled out the text
                    value_initializer=' = '+value_initializer.strip()
            else: #initialiser does not contains links
                value_initializer=temp_initializer
                #clean up excess space in initialisation script
                value_initializer=" ".join(value_initializer.split())
                value_initializer=" "+value_initializer.strip()
            #print('value_initializer: %s' % value_initializer)



        output_string+='\n```cpp\n%s%s\n```\n' % (markdown_any_tag(self.definition).strip(),value_initializer)


        output_string+='\n\n%s' % markdown_any_tag(self.briefdescription).strip()
        output_string+='\n\n%s' % markdown_any_tag(self.detaileddescription).strip()

        if len(self.seealso)>0:
            output_string += self.seealso #Note, untested

        if args.debug:
            output_string+='\n\n<!-- [%s %s](#%s) -->' % (self.type, self.name,self.id)
            output_string+='\n<!-- kind: %s -->' % markdown_any_tag(self.kind).strip()
            output_string+='\n<!-- prot: %s -->' % markdown_any_tag(self.prot).strip()
            output_string+='\n<!-- static: %s -->' % markdown_any_tag(self.static).strip()
            output_string+='\n<!-- mutable: %s -->' % markdown_any_tag(self.mutable).strip()
            output_string+='\n<!-- definition: %s -->' % markdown_any_tag(self.definition).strip()
            output_string+='\n<!-- detaileddescription: %s -->' % markdown_any_tag(self.detaileddescription).strip()
            output_string+='\n<!-- briefdescription: %s -->\n' % markdown_any_tag(self.briefdescription).strip()
            output_string+='\n<!-- argsstring: %s -->\n' % markdown_any_tag(self.argsstring).strip()
            output_string+='\n<!-- type: %s -->\n' % markdown_any_tag(self.type).strip()
            output_string+='\n<!-- id: %s -->\n' % markdown_any_tag(self.id).strip()
            output_string+='\n<!-- name: %s -->\n' % markdown_any_tag(self.name).strip()
            output_string+='\n<!-- initializer: %s -->\n' % markdown_any_tag(self.initializer).strip()


        return output_string




class cppInnerClass: #Data structures
    def __init__(self):
        self.id= 'UNKNOWN'
        self.name = 'UNKNOWN'
        self.prot = 'UNKNOWN'
        self.kind = 'innerclass'


class cppTypeDefParams:
    def __init__(self):
        self.type= ''
        self.name = ''
        self.description = 'UNKNOWN'


class cppTypeDef:
    def __init__(self):
        self.kind='typedef'
        self.id = 'UNKNOWN'
        self.prot= 'UNKNOWN'
        self.static = 'UNKNOWN'
        self.name = 'UNKNOWN'
        self.type = 'UNKNOWN'
        self.definition = 'UNKNOWN'
        self.argsstring = 'UNKNOWN'
        self.briefdescription = 'UNKNOWN'
        self.detaileddescription = 'UNKNOWN'
        self.seealso = ''
        self.inbodydescription = 'UNKNOWN'
        self.params = []

    def import_from_doxygen_class_file(self, aTag):
        #print('start:typedef:import_from_doxygen_class_file')
        #print(aTag)
        #print(aTag.attrib)
        self.id = aTag.attrib['id']
        self.prot= aTag.attrib['prot']
        self.static= aTag.attrib['static']

        self.name = aTag.find('name').text
        self.type = aTag.find('type') #needs further processing
        self.definition = aTag.find('definition') #needs further processing
        self.argsstring = aTag.find('argsstring') #needs further processing
        self.briefdescription = aTag.find('briefdescription') #needs more processing
        self.detaileddescription = aTag.find('detaileddescription')  # needs more processing
        self.seealso = seealso_from_tag(self.detaileddescription)
        self.inbodydescription = aTag.find('inbodydescription')  # needs more processing
        self.params=[]

        name_nodes=aTag.findall(".//parameterlist[@kind='param']/parameteritem/parameternamelist/")
        #print(name_nodes)
        for item in name_nodes:
            newParam=cppTypeDefParams()
            newParam.name=item.text
            matchstring=".//parameterlist[@kind='param']/parameteritem/parameternamelist/[parametername='%s']/../parameterdescription" % newParam.name
            newParam.description = aTag.findall(matchstring)
            if len(newParam.description)==1:
                newParam.description=newParam.description[0]  #requires more processing
            #print('newParam.description: %s' % markdown_any_tag(newParam.description))
            #print('newParam.name: %s' % newParam.name)
            self.params.append(newParam)


    def markdown_overview(self):
        # render the typedef at top section -
        #output_string='%s [%s](#%s) &emsp;- %s' % (markdown_any_tag(self.type).strip(),self.name,self.id,markdown_any_tag(self.briefdescription).strip())
        #output_string+='\n\ntype: %s' % markdown_any_tag(self.type).strip()
        #output_string+='\n\nid: %s' % self.id
        #output_string+='\n\ndefinition: %s' % markdown_any_tag(self.definition).strip()
        #output_string+='\n\nargsstring: %s' % markdown_any_tag(self.argsstring).strip()

        output_string='\n%s [%s](#%s) | %s' % (markdown_any_tag(self.type).strip(),self.name,self.id,markdown_any_tag(self.briefdescription).strip())

        return output_string

    def markdown(self):
        # TBD
        output_string=''
        output_string+='\n\n### typedef %s {#%s}\n' % (self.name, self.id)
        output_string+='\n```cpp\n%s\n```\n' % markdown_any_tag(self.definition).strip()
        output_string+='\n\n%s' % markdown_any_tag(self.briefdescription).strip()
        output_string+='\n\n%s' % markdown_any_tag(self.detaileddescription).strip()

        #generate params list
        if len(self.params)>0:
            output_string+='\n\n**Parameters**\n'
            params_string=''
            for param in self.params:
                params_string+='\n* **%s** - %s' % (markdown_any_tag(param.name).strip(),markdown_any_tag(param.description).strip())

            output_string+=params_string

        if len(self.seealso)>0:
            output_string+=self.seealso

        return output_string



class cppEnumValue:
    def __init__(self):
        self.id = 'UNKNOWN'
        self.prot= 'UNKNOWN'
        self.name = 'UNKNOWN'
        self.initializer = ''
        self.briefdescription='UNKNOWN'
        self.detaileddescription='UNKNOWN'


    def import_from_doxygen_class_file(self, aTag):
        #print(aTag)
        #print(aTag.attrib)
        #print(aTag.tag)
        self.id = aTag.attrib['id']
        self.prot= aTag.attrib['prot']
        self.name = aTag.find('name').text
        initializer = aTag.find('initializer')
        if initializer is not None:
            initializer=initializer.text
            self.initializer=''.join(initializer.split()) #remove all whitespace
        self.briefdescription = aTag.find('briefdescription') #needs more processing
        self.detaileddescription = aTag.find('detaileddescription') #needs more processing



class cppEnum:
    def __init__(self):
        self.kind='enum'
        self.id = 'UNKNOWN'
        self.prot= 'UNKNOWN'
        self.static = 'UNKNOWN'
        self.name = 'UNKNOWN'
        self.briefdescription='UNKNOWN'
        self.detaileddescription='UNKNOWN'
        self.inbodydescription='UNKNOWN'
        self.seealso=''
        self.enum_values=[]
        self.location='' # Globals only

    def import_from_doxygen_class_file(self, aTag):
        #print('start:enum:import_from_doxygen_class_file')
        #print(aTag)
        #print(aTag.attrib)
        self.name = aTag.find('name').text
        self.id = aTag.attrib['id']
        self.prot= aTag.attrib['prot']
        self.static= aTag.attrib['static']
        self.briefdescription = aTag.find('briefdescription') #needs further processing
        self.detaileddescription = aTag.find('detaileddescription') #needs further processing
        self.seealso = seealso_from_tag(self.detaileddescription)
        self.inbodydescription = aTag.find('inbodydescription') #needs further processing

        location = aTag.find('location')
        if location is not None:
            self.location=location.attrib['file']

        enumvalue_nodes = aTag.findall("enumvalue")
        #print('enumvalue_nodes: %s' % enumvalue_nodes)
        if len(enumvalue_nodes)==0:
            print('Zero enumvalue_nodes - problem parsing')
        for child in enumvalue_nodes: # all the items
            newEnumValue=cppEnumValue()
            newEnumValue.import_from_doxygen_class_file(child)
            self.enum_values.append(newEnumValue)

    def markdown_overview(self):
        # render the enum at top section - just names with link down to the full description
        # Generate enum in HTML list as this is the only way to get good indentation in markdown.

        """
        enum_brief_description=markdown_any_tag(self.briefdescription).strip()
        output_string='\nenum [%s](#%s) {<ul><li style="list-style-type: none;">' % (self.name,self.id)

        #generate params list
        if len(self.enum_values)>0:
            first_iter = True
            for enum_value in self.enum_values:
                if first_iter:
                    first_iter = False
                else:
                    output_string+=', '
                output_string+='<a href="#%s">%s%s</a>' % (enum_value.id,enum_value.name,enum_value.initializer)
            #output_string+='</li>\n}\n\n'
        """
        output_string='\nenum [%s](#%s) | %s' % (self.name,self.id,markdown_any_tag(self.briefdescription).strip())

        return output_string

    def markdown(self,aDisplayInclude=False):
        """
        Markdown for the enum, with details
        """
        output_string=''
        output_string+='\n\n### enum %s {#%s}\n' % (self.name,self.id)

        if aDisplayInclude:
            output_string+='\n```\n#include: %s\n```\n' % (self.location)

        output_string+='\n\n%s' % markdown_any_tag(self.briefdescription).strip()

        output_string+='\n\n%s' % markdown_any_tag(self.detaileddescription).strip()

        if len(self.enum_values)>0:
            output_string+='\n\nValue | Description\n--- | ---'
            for enum_value in self.enum_values:
                output_string+='\n<span id="%s"></span> `%s` | %s %s' % (enum_value.id,enum_value.name, markdown_any_tag(enum_value.briefdescription).strip(),markdown_any_tag(enum_value.detaileddescription).strip())


        if len(self.seealso)>0:
            output_string+=self.seealso


        if args.debug:
            output_string+='\n\n'
            output_string+='<!-- inbodydescription: %s --> \n' % markdown_any_tag(self.inbodydescription).strip()
            output_string+='<!-- prot: %s -->\n' % self.prot
            output_string+='<!-- static: %s -->\n' % self.static

        return output_string




class cppFunctionParams:
    def __init__(self):
        self.type= ''
        self.declname = ''
        self.defval = ''
        self.description = ''


class cppFunction:
    def __init__(self):
        self.name = 'UNKNOWN'
        self.id = 'UNKNOWN'
        self.prot= 'UNKNOWN'
        self.static = 'UNKNOWN'
        self.const = 'UNKNOWN'
        self.explicit='UNKNOWN'
        self.inline='UNKNOWN'
        self.virt='UNKNOWN'
        self.return_type='UNKNOWN'
        self.return_type_comment='UNKNOWN'
        self.definition='UNKNOWN'
        self.argsstring='UNKNOWN'
        self.argsstring2='' #Fully linked version of self.argsstring
        self.briefdescription='UNKNOWN'
        self.detaileddescription='UNKNOWN'
        self.inbodydescription='UNKNOWN'
        self.constructor_func = 'false'
        self.destructor_func = 'false'
        self.seealso = ''
        self.params=[]
        self.location='' #For global functions only


    def import_from_doxygen_class_file(self, func_xml):
        #print(func_xml.attrib)

        self.name = func_xml.find('name').text

        self.id = func_xml.attrib['id']
        self.prot= func_xml.attrib['prot']
        self.static= func_xml.attrib['static']
        self.const= func_xml.attrib['const']
        self.explicit= func_xml.attrib['explicit']
        self.inline= func_xml.attrib['inline']
        self.virt= func_xml.attrib['virt']

        self.return_type = func_xml.find('type') #needs more processing
        self.definition = func_xml.find('definition') #needs more processing
        self.argsstring = func_xml.find('argsstring') #needs more processing
        self.briefdescription = func_xml.find('briefdescription') #needs more processing
        self.detaileddescription = func_xml.find('detaileddescription') #needs further parsing
        self.seealso = seealso_from_tag(self.detaileddescription)
        self.inbodydescription = func_xml.find('inbodydescription') #needs more processing

        #Get return type comment
        return_nodes = func_xml.findall(".//simplesect[@kind='return']")
        comment_node=''
        if len(return_nodes)>0:
            comment_node=return_nodes[0]
        if len(return_nodes)>1:
            print('UNHANDLED (incorrect): More than one return node from function')
        self.return_type_comment=comment_node # needs more processing (is simplesect data.)

        #Populate params object
        params=func_xml.findall('param')
        #print('Lenparams: %s' % len(params))
        for param in params:
            newParam=cppFunctionParams()
            newParam.type=param.find('type')
            if newParam.type is None:
                newParam.type=''
            newParam.declname=param.find('declname')
            if newParam.declname is None:
                newParam.declname=''
            else:
                newParam.declname=newParam.declname.text #the declname will never be anything but text
                #Find matching text string for the description name
                matchstring=".//parameterlist[@kind='param']/parameteritem/parameternamelist/[parametername='%s']/../parameterdescription" % newParam.declname
                parameterdescription_list = func_xml.findall(matchstring)
                if len(parameterdescription_list)==1:
                    newParam.description=parameterdescription_list[0]  #requires more processing
                else:
                   newParam.description=''

                #print('newParam.description: %s ' % markdown_any_tag(newParam.description).strip())
            # Default value for param (store as expanded value)
            newParam.defval=param.find('defval')
            if newParam.defval is None:
                newParam.defval=''
            else:
                newParam.defval=markdown_any_tag(newParam.defval)


            self.params.append(newParam)

        location = func_xml.find('location')
        if location is not None:
            self.location=location.attrib['file']


        #Calculate argument string from parameters (for fully linked prototype)
        self.argsstring2=''
        first_loop = True
        for param in self.params:
            if first_loop:
                first_loop = False
            else:
                self.argsstring2+=', '

            self.argsstring2+=markdown_any_tag(param.type).strip()
            if param.declname:
                self.argsstring2+=' '+markdown_any_tag(param.declname).strip()
            if param.defval:
                self.argsstring2+='='+markdown_any_tag(param.defval).strip()
        if not self.argsstring2:
            self.argsstring2=markdown_any_tag(self.argsstring)
        else:
            self.argsstring2='(%s)' % self.argsstring2
            trail_argsstring = markdown_any_tag(self.argsstring).rsplit(')',1)[1].strip()
            if trail_argsstring:
                self.argsstring2=self.argsstring2+trail_argsstring

        #print('argstring2: %s\n\n' % self.argsstring2)


    def markdown_overview(self):
        # Print function in overview
        return_type=markdown_any_tag(self.return_type).strip()
        if not return_type:
            return_type = '&nbsp;'
        output_string='%s | [%s](#%s) %s | %s\n\n' % (return_type, self.name, self.id, self.argsstring2.strip(),markdown_any_tag(self.briefdescription).strip())
        return output_string



    def markdown(self,aDisplayInclude=False):
        functionstring=''
        functionstring+='\n\n### %s() {#%s}\n' % (self.name,self.id)
        if aDisplayInclude:
            functionstring+='\n```\n#include: %s\n```\n' % (self.location)
        functionstring+='```cpp\n%s%s\n```\n' % (markdown_any_tag(self.definition).strip(),markdown_any_tag(self.argsstring).strip())

        # Description
        functionstring+='\n\n%s' % markdown_any_tag(self.briefdescription).strip()
        functionstring+='\n\n%s' % markdown_any_tag(self.detaileddescription).strip()

        #generate params list
        if len(self.params)>0:
            functionstring+='\n\n**Parameters**\n'
            params_string=''
            for param in self.params:
                declname_string=markdown_any_tag(param.declname).strip()
                if declname_string: #render declname string in bold, if it exists.
                    declname_string='**%s**' % declname_string

                # Fix spacing for ref/pointer and ensure pointer symbol (*) escaped
                param.type=markdown_any_tag(param.type).strip()
                if param.type.endswith('*') or param.type.endswith('&'): # A ref or pointer
                    temp=param.type.rsplit(' ',1)
                    escape_text=''
                    if param.type.endswith('*'):
                        escape_text='\\'
                    temp=temp[0]+escape_text+temp[1]
                    param.type=temp
                params_string+='\n* %s %s - %s' % (markdown_any_tag(param.type).strip(),declname_string,markdown_any_tag(param.description).strip())

            functionstring+=params_string

        #generate return type
        return_text=markdown_any_tag(self.return_type).strip()
        if len(return_text)>0 and not return_text=='void':
            functionstring+='\n\n**Returns**\n'
            functionstring+='\n&emsp;%s - %s' % (markdown_any_tag(self.return_type).strip(),markdown_any_tag(self.return_type_comment).strip())
            #print('debug:cppFunction:markdown:return_text: %s' % functionstring)

        if len(self.seealso)>0:
            functionstring+=self.seealso

        if args.debug:
            functionstring+='\n\n'
            functionstring+='<!-- inbodydescription: %s --> \n' % markdown_any_tag(self.inbodydescription).strip()
            functionstring+='<!-- return_type: %s -->\n' % markdown_any_tag(self.return_type).strip()
            functionstring+='<!-- return_type_comment: %s -->\n' % markdown_any_tag(self.return_type_comment).strip()
            functionstring+='<!-- prot: %s -->\n' % markdown_any_tag(self.prot).strip()
            functionstring+='<!-- static: %s -->\n' % markdown_any_tag(self.static).strip()
            functionstring+='<!-- const: %s -->\n' % markdown_any_tag(self.const).strip()
            functionstring+='<!-- explicit: %s -->\n' % markdown_any_tag(self.explicit).strip()
            functionstring+='<!-- virt: %s -->\n' % markdown_any_tag(self.virt).strip()
            functionstring+='<!-- inline: %s -->\n' % markdown_any_tag(self.inline).strip()

        return functionstring


class cppClass:
    def __init__(self):
        self.kind = 'UNKNOWN'
        self.compound_name = 'UNKNOWN'
        self.name = 'UNKNOWN'
        self.include_file = 'UNKNOWN'
        self.id = 'UNKNOWN'
        self.prot= 'UNKNOWN'
        self.abstract = 'UNKNOWN'
        self.language = 'cpp'
        self.detaileddescription=''
        self.briefdescription=''
        self.seealso = ''
        self.derived_components=[]
        self.public_attributes=[]
        self.protected_attribs=[]
        self.public_static_attributes=[]
        self.protected_functions=[]
        self.public_functions=[]
        self.protected_constructor_destructors=[]
        self.public_constructor_destructors=[]
        self.constructor_destructor_functions=[]
        self.public_static_functions=[]
        self.public_enums=[]
        self.public_typedef=[]
        self.inner_classes=[]



    def import_doxygen_class_file(self, name, full_filename):
        tree = ET.parse(full_filename)
        root = tree.getroot()

        kind=root[0].attrib['kind']
        if not (kind=='class' or kind=='struct'):
           raise Exception('File not of correct type - reports wrong kind')
        self.kind=kind
        self.compound_name = root[0].find('compoundname').text
        self.name = self.compound_name.split('::')[-1]
        try:
            self.include_file = root[0].find('includes').text
        except:
            pass # It has a default empty value.

        self.detaileddescription=root[0].find('detaileddescription')
        self.briefdescription=root[0].find('briefdescription')
        self.seealso = seealso_from_tag(self.detaileddescription)


        # Get all inner classes/struct defined in this class - these are links to separate docs
        inner_class_nodes=root.findall(".//innerclass")
        #print(inner_class_nodes)
        for item in inner_class_nodes:
            newInnerClass=cppInnerClass()
            newInnerClass.name=item.text
            newInnerClass.id=item.attrib['refid']
            newInnerClass.prot=item.attrib['prot']
            self.inner_classes.append(newInnerClass)


        sections = root[0].findall("sectiondef")
        for section in sections:
            section_kind=section.attrib['kind']
            #print('section_kind: %s' % section_kind)
            if section_kind.startswith('private-'):
                #Ignore - private
                continue
            if section_kind=='public-func' or section_kind=='protected-func' or section_kind=='public-static-func':
                for child in section:
                    newFunction=cppFunction()
                    #print(child.attrib)
                    newFunction.import_from_doxygen_class_file(child)
                    #print('debug:newFunction.name' % newFunction.name)
                    if section_kind=='public-func':
                        if newFunction.name==self.name or newFunction.name=='~%s' % self.name:
                            #public constructor
                            self.public_constructor_destructors.append(newFunction)
                        else:
                            self.public_functions.append(newFunction)
                    elif section_kind=='protected-func':
                        if newFunction.name==self.name or newFunction.name=='~%s' % self.name:
                            #protected constructor
                            self.protected_constructor_destructors.append(newFunction)
                        else:
                            self.protected_functions.append(newFunction)
                    elif section_kind=='public-static-func':
                        self.public_static_functions.append(newFunction)


            elif section_kind=='public-attrib':
                for child in section:
                    newAttribute=cppAttribute()
                    newAttribute.import_from_doxygen_class_file(child)
                    self.public_attributes.append(newAttribute)


            elif section_kind=='protected-attrib':
                for child in section:
                    newAttribute=cppAttribute()
                    newAttribute.import_from_doxygen_class_file(child)
                    self.protected_attribs.append(newAttribute)


            elif section_kind=='public-static-attrib':
                for child in section:
                    newAttribute=cppAttribute()
                    newAttribute.import_from_doxygen_class_file(child)
                    self.public_static_attributes.append(newAttribute)



            elif section_kind=='public-type':
                for child in section:
                    if child.attrib['kind']=='enum':
                        newType=cppEnum()
                        newType.import_from_doxygen_class_file(child)
                        self.public_enums.append(newType)
                    elif child.attrib['kind']=='typedef':
                        newType=cppTypeDef()
                        newType.import_from_doxygen_class_file(child)
                        self.public_typedef.append(newType)
                    else:
                        print('UNHANDLED public-type: %s' % child.attrib['kind'])
            else:
                print('UNHANDLED section: XX%sYY' % section_kind)



    def markdown_overview_members(self):
        outputstring=''


        ## Data structures (inner classes - actual classes in other docs)
        if len(self.inner_classes)>0:
            outputstring+='\n\n## Data Structures\n\n'
            for item in self.inner_classes:
                outputstring+='\n\n'
                link_name=item.name.split('::')[-1]
                link_url='%s.md' %item.id
                #not using Prot
                outputstring+='struct [%s](%s)' % (link_name,link_url)
                #print(outputstring)



        ## Public Types
        if len(self.public_enums)>0 or len(self.public_typedef)>0:
            outputstring+='\n\n## Public Types\n\n'
            outputstring+='\nType | Description'
            outputstring+='\n--- | ---'
            if len(self.public_enums)>0:
                for the_type in self.public_enums:
                    ##outputstring+='\n\n'
                    outputstring+=the_type.markdown_overview()
            if len(self.public_typedef)>0:
                for the_type in self.public_typedef:
                    #outputstring+='\n\n'
                    outputstring+=the_type.markdown_overview()

        ## Functions
        if len(self.public_functions)>0 or len(self.public_constructor_destructors)>0:
            outputstring+='\n\n## Public Member Functions\n\n'
            outputstring+='\n\nType | Name | Description'
            outputstring+='\n---: | --- | ---'
            for the_item in self.public_constructor_destructors:
                outputstring+='\n%s' % the_item.markdown_overview().strip()
            for the_item in self.public_functions:
                outputstring+='\n%s' % the_item.markdown_overview().strip()


        ## Data Fields/Public attributes
        if len(self.public_attributes)>0:
            outputstring+='\n\n## Data Fields\n'
            for the_item in self.public_attributes:
                outputstring+='\n\n%s' % the_item.markdown_overview().strip()


        if len(self.public_static_attributes)>0:
            outputstring+='\n\n## Static Public Attributes\n'
            for the_item in self.public_static_attributes:
                outputstring+=the_item.markdown_overview()


        # Static Public Functions
        if len(self.public_static_functions)>0:
            outputstring+='\n\n## Static Public Member Functions\n\n'
            outputstring+='\n\nType | Name | Description'
            outputstring+='\n---: | --- | ---'
            for the_item in self.public_static_functions:
                outputstring+='\n%s' % the_item.markdown_overview().strip()

        if len(self.protected_functions)>0 or len(self.protected_constructor_destructors)>0:
            outputstring+='\n\n## Protected Member Functions\n'
            outputstring+='\n\nType | Name | Description'
            outputstring+='\n---: | --- | ---'
            for the_item in self.protected_constructor_destructors:
                outputstring+='\n%s' % the_item.markdown_overview().strip()
            for the_item in self.protected_functions:
                outputstring+='\n%s' % the_item.markdown_overview().strip()

        # Protected Attributes
        if len(self.protected_attribs)>0:
            outputstring+='\n\n## Protected Attributes\n'
            for the_item in self.protected_attribs:
                outputstring+='[%s %s](#%s)\n\n' % (the_item.type, the_item.name,the_item.id)

        return outputstring


    def markdown(self):
        outputstring=''
        kind_name='Class'
        if self.kind=='struct':
            kind_name='Struct'
        outputstring+='# %s %s Reference\n' % (self.compound_name, kind_name)

        outputstring+='`#include: %s`\n' % self.include_file

        outputstring+='\n----' # Description
        outputstring+='\n\n%s' % markdown_any_tag(self.briefdescription)
        outputstring+='\n\n%s' % markdown_any_tag(self.detaileddescription)
        if len(self.seealso)>0:
            outputstring+=self.seealso


        # Overview Section
        outputstring+=self.markdown_overview_members()
        outputstring+='\n\n'

        ## Detail sections

        ## Constructors and Destructor docs
        if len(self.public_constructor_destructors)>0 or len(self.protected_constructor_destructors)>0:
            outputstring+='\n\n## Constructor & Destructor Documentation\n\n'
            for the_function in self.public_constructor_destructors:
                    outputstring+=the_function.markdown()
            for the_function in self.protected_constructor_destructors:
                    outputstring+=the_function.markdown()

        if len(self.public_typedef)>0:
            outputstring+='\n\n## Member Typdef Documentation\n\n'
            for the_type in self.public_typedef:
                outputstring+=the_type.markdown()

        if len(self.public_enums)>0:
            outputstring+='\n\n## Member Enumeration Documentation\n\n'
            for the_type in self.public_enums:
                outputstring+=the_type.markdown()

        ## Member Function Documentation
        if len(self.public_functions)>0 or len(self.public_static_functions)>0:
            outputstring+='\n\n## Member Function Documentation\n\n'
            if len(self.public_functions)>0:
                #print(self.public_functions)
                for the_function in self.public_functions:
                    outputstring+=the_function.markdown()

            if len(self.public_static_functions)>0:
                #print(self.public_static_functions)
                for the_function in self.public_static_functions:
                    outputstring+=the_function.markdown()

        ## Protected Member Functions
        if len(self.protected_functions)>0:
            outputstring+='\n\n## Protected Member Functions\n'
            for the_function in self.protected_functions:
                outputstring+=the_function.markdown()

        ## Member Data Documentation
        if len(self.protected_attribs)>0:
            outputstring+='\n\n## Member Data Documentation\n\n'
            for the_item in self.protected_attribs:
                outputstring+=the_item.markdown()

        ## Field Documentation
        if len(self.public_static_attributes)>0 or len(self.public_attributes)>0:
            outputstring+='\n\n## Field Documentation\n\n'
            if len(self.public_static_attributes)>0:
                for the_item in self.public_static_attributes:
                    outputstring+=the_item.markdown()

            if len(self.public_attributes)>0:
                for the_item in self.public_attributes:
                    outputstring+=the_item.markdown()



        outputstring=cleanup_markdown_string(outputstring)

        return outputstring


class cppNamespace:
    def __init__(self):
        self.kind = 'UNKNOWN'
        self.compoundname = 'UNKNOWN'
        self.name = 'UNKNOWN'
        self.id = 'UNKNOWN'
        self.language = 'cpp'
        self.detaileddescription=''
        self.briefdescription=''
        self.functions=[]
        self.enumerations=[]
        self.inner_classes=[]


    def import_doxygen_namespace_file(self, name, full_filename):
        tree = ET.parse(full_filename)
        root = tree.getroot()

        kind=root[0].attrib['kind']

        if not (kind=='namespace'):
           raise Exception('File not of correct type - reports wrong kind: %s' % kind)
        self.kind=kind
        self.compound_name = root[0].find('compoundname').text
        self.name = self.compound_name.split('::')[-1]

        self.detaileddescription=root[0].find('detaileddescription')
        self.briefdescription=root[0].find('briefdescription')
        self.seealso = seealso_from_tag(self.detaileddescription)


        # Get all inner classes/struct defined in this class - these are links to separate docs
        inner_class_nodes=root.findall(".//innerclass")
        #print(inner_class_nodes)
        for item in inner_class_nodes:
            newInnerClass=cppInnerClass()
            newInnerClass.name=item.text
            newInnerClass.id=item.attrib['refid']
            newInnerClass.prot=item.attrib['prot']
            self.inner_classes.append(newInnerClass)


        sections = root[0].findall("sectiondef")
        for section in sections:
            section_kind=section.attrib['kind']
            #print('section_kind: %s' % section_kind)
            if section_kind=='enum':
                for child in section:
                    if child.attrib['kind']=='enum':
                        newType=cppEnum()
                        newType.import_from_doxygen_class_file(child)
                        self.enumerations.append(newType)
                    else:
                        print('UNHANDLED enum-kind: %s' % child.attrib['kind'])
            elif section_kind=='func':
                for child in section:
                    if child.attrib['kind']=='function':
                        newType=cppFunction()
                        newType.import_from_doxygen_class_file(child)
                        self.functions.append(newType)
                    else:
                        print('UNHANDLED function-kind: %s' % child.attrib['kind'])

            else:
                print('UNHANDLED section: XX%sYY' % section_kind)


    def markdown(self):
        outputstring=''
        kind_name='Namespace'
        outputstring+='# %s %s Reference\n' % (self.compound_name, kind_name)

        outputstring+='\n----' # Description
        outputstring+='\n\n%s' % markdown_any_tag(self.briefdescription).strip()
        outputstring+='\n\n%s' % markdown_any_tag(self.detaileddescription).strip()
        if len(self.seealso)>0:
            outputstring+=self.seealso

        ## Data Structures
        # TODO: Perhaps do as table "Type | Name | Description"
        #     : Can't do now as don't have access to linked item discription
        if len(self.inner_classes)>0:
            outputstring+='\n\n## Data Structures\n'
            for the_structure in self.inner_classes:
                outputstring+='\n* [%s](%s.md)' % (the_structure.name,the_structure.id)

        if len(self.enumerations)>0:
            outputstring+='\n\n## Enumerations\n'
            outputstring+='\nType | Description'
            outputstring+='\n--- | ---'
            for the_enum in self.enumerations:
                outputstring+='\n'+the_enum.markdown_overview().strip()


        if len(self.functions)>0:
            outputstring+='\n\n## Functions\n'
            outputstring+='\nType | Name | Description'
            outputstring+='\n--- | --- | ---'
            for the_function in self.functions:
                outputstring+='\n'+the_function.markdown_overview().strip()


        if len(self.enumerations)>0:
            outputstring+='\n\n## Enumeration Type Documentation\n'
            for the_enum in self.enumerations:
                outputstring+=the_enum.markdown(aDisplayInclude=True)

        if len(self.functions)>0:
            outputstring+='\n\n## Function Documentation\n'
            for the_function in self.functions:
                outputstring+=the_function.markdown(aDisplayInclude=True)


        outputstring=cleanup_markdown_string(outputstring)

        return outputstring




# Start actual execution
parser = argparse.ArgumentParser(description='Generate markdown from doxygen XML output.')
parser.add_argument("-d", "--debug", default='', help="Any argument used here will include debug info in markdown output - ie some strings for various objects.")
parser.add_argument("input_dir", help="Directory where xml input can be found.")
parser.add_argument("output_dir", help="Directory where markdown output should be saved.")
args = parser.parse_args()

# Load all the xml files in the dir
DOXYGEN_ROOT_DIR= args.input_dir
DOXYGEN_XML_DIR = args.input_dir + "/xml"
DOXYGEN_OUTPUT_DIR = args.output_dir + '/markdown'


# Check if XML file path exists
if not os.path.exists(DOXYGEN_ROOT_DIR):
    print('Docs directory does not exist.')
    exit()
if not os.path.exists(DOXYGEN_XML_DIR):
    print('XML directory does not exist (created by doxygen).')
    exit()
if not os.path.exists(DOXYGEN_OUTPUT_DIR):
    #make output dir
    os.makedirs(DOXYGEN_OUTPUT_DIR)



skipped_files=[] # Files we have CHOSEN not to parse
not_handled_files=[] # Files we don't handle, but maybe we should (catches new file types)
for root, dirs, files in os.walk(DOXYGEN_XML_DIR, topdown=False):
    for name in files:
        current_filename = os.path.join(root, name)

        if not name.endswith('.xml'):
            skip_string=" %s - (not XML)" % current_filename
            skipped_files.append(skip_string)
            continue
        if name.endswith('_8h.xml'):
            skip_string=" %s - (header file)" % current_filename
            skipped_files.append(skip_string)
            continue
        if name.endswith('_8cpp.xml'):
            skip_string=" %s - (cpp file)" % current_filename
            skipped_files.append(skip_string)
            continue
        if name.startswith('dir_'):
            skip_string=" %s - (directory listing)" % current_filename
            skipped_files.append(skip_string)
            continue
        if name.endswith('index.xml'):
            skip_string=" %s - (index page)" % current_filename
            skipped_files.append(skip_string)
            continue
        if name.endswith('namespacemavsdk.xml'):
            print("  Generating: %s (namespace xml index file)" % current_filename)
            currentNamespace=cppNamespace()
            currentNamespace.import_doxygen_namespace_file(name,current_filename)

            markdown_string=currentNamespace.markdown()
            outputfile_name=DOXYGEN_OUTPUT_DIR+'/'+name[:-4]+'.md'

            #print('OUTPUTFILENAME: %s' % outputfile_name)
            with open(outputfile_name, 'w') as the_file:
                the_file.write(markdown_string)
            continue
        if name.startswith('class'):
            print("  Generating: %s (class xml)" % current_filename)
            currentClass=cppClass()
            currentClass.import_doxygen_class_file(name,current_filename)
            markdown_string=currentClass.markdown()
            outputfile_name=DOXYGEN_OUTPUT_DIR+'/'+name[:-4]+'.md'

            #print('OUTPUTFILENAME: %s' % outputfile_name)
            with open(outputfile_name, 'w') as the_file:
                the_file.write(markdown_string)
            continue

        if name.startswith('struct'):
            print("  Generating: %s (struct xml)" % current_filename)
            currentClass=cppClass()
            currentClass.import_doxygen_class_file(name,current_filename)
            markdown_string=currentClass.markdown()
            #print(markdown_string)
            outputfile_name=DOXYGEN_OUTPUT_DIR+'/'+name[:-4]+'.md'

            #print('OUTPUTFILENAME: %s' % outputfile_name)
            with open(outputfile_name, 'w') as the_file:
                the_file.write(markdown_string)
            continue

        #File is not handled.
        not_handled_files.append(current_filename)


if skipped_files:
    print("Skipped files:")
    for item in skipped_files:
        print('  %s' % item)
if not_handled_files:
    print("Unhandled files - review and add to skipped list if not needed:")
    for item in not_handled_files:
        print('  %s' % item)
