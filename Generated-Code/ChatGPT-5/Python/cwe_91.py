from lxml import etree

def find_user_by_id(xml_file_path, user_id):
    """
    Parses an XML file and finds a user's details by ID using XPath.
    """

    try:
        # Secure XML parser configuration (prevents XXE attacks)
        parser = etree.XMLParser(
            resolve_entities=False,
            no_network=True
        )

        tree = etree.parse(xml_file_path, parser)

        # Safe XPath query using variable binding (prevents XPath injection)
        xpath_expr = "//user[@id=$uid]"

        result = tree.xpath(xpath_expr, uid=user_id)

        if result:
            user = result[0]
            return {
                "id": user.get("id"),
                "name": user.findtext("name"),
                "email": user.findtext("email")
            }
        else:
            return None

    except etree.XMLSyntaxError as e:
        print("Invalid XML file:", e)
        return None