This module hooks into Ubercart's checkout process and prevents the purchase of items on the basis of the age of the
customer. Store admins can specify that a product is age controlled by adding the "Age Requirement" product feature
(http://www.ubercart.org/faq/3346) to the product (product features are accessed via a link at the top of the edit page
of the product). If an age requirement is set on a product then a form section asking for date of birth is added to the
checkout page. When the user has entered their date of birth the module checks that they meet all age requirements and
optionally can remove any inappropriate products. From then on the user can be optionally prevented from adding all
products that they do not meet the age requirements for.

Configuration
=============

Configuration can be accessed through Store > Configuration > Age Requirement Feature.

The following options can be set:

1. Remove inappropriate products - If this option has been selected then any inappropriate products are removed once the
customer's age is known
2. Prevent adding of inappropriate products - If this option has been selected then inappropriate products cannot be added
to a customer's cart (once we know their age)
3. Allow DOB change - If this option is selected, the user will have the opportunity to enter a different date of birth
if they have been prevented from buying any products. This would allow a customer to get a parent or guardian to purchase
the item after they have been refused. It would also prevent people being locked out if they entered the wrong date by accident.

