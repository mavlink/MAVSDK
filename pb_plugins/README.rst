===================================
Autogenerator for the dronecore sdk
===================================

Install instructions
~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    pip install -r requirements.txt
    pip install -e .


How to use it
~~~~~~~~~~~~~

.. code-block:: bash

    protoc --plugin=protoc-gen-custom=$(which dcsdkgen) \
            -I../protos/action \
            --custom_out=. \
            --custom_opt=swift \
            ../protos/action/action.proto

