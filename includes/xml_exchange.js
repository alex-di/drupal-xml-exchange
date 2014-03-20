
var exp = /form-item-([A-Za-z\-]{1,})-set-(\w+)/;
var expF = /field$/;
jQuery(document).ready(function() {
    jQuery.each(jQuery('#xml-exchange-admin-edit-feed-form fieldset select'), function() {
        var classStr = jQuery(this).parent().attr('class');
        var field = exp.exec(classStr)[1];
        if (expF.test(jQuery(this).attr('id'))) {
            jQuery('.form-item-' + field + '-set-predefined').hide();
            jQuery('.form-item-' + field + '-set-type').hide();
            if (jQuery(this).val() != '0' && jQuery(this).val() != 'def') {
                jQuery('.form-item-' + field + '-set-type').show();
            }
            if (jQuery(this).val() == 'def')
                jQuery('.form-item-' + field + '-set-predefined').show();
        }
        if (jQuery(this).attr('id').indexOf('type') !== -1) {
            jQuery('.form-item-' + field + '-set-php').hide();
            jQuery('.form-item-' + field + '-set-texttoterm').hide();
            jQuery('.form-item-' + field + '-set-' + jQuery(this).val()).show();
        }
        jQuery(this).change(function() {
            var classStr = jQuery(this).parent().attr('class');
            var field = exp.exec(classStr)[1];
            if (expF.test(jQuery(this).attr('id'))) {
                jQuery('.form-item-' + field + '-set-predefined').hide();
                jQuery('.form-item-' + field + '-set-type').hide();
                if (jQuery(this).val() != '0' && jQuery(this).val() != 'def') {
                    jQuery('.form-item-' + field + '-set-type').show();
                }
                if (jQuery(this).val() == 'def')
                    jQuery('.form-item-' + field + '-set-predefined').show();
            }
            else if (jQuery(this).attr('id').indexOf('type') !== -1) {
                // Это тип связи
                jQuery('.form-item-' + field + '-set-php').hide();
                jQuery('.form-item-' + field + '-set-texttoterm').hide();
                jQuery('.form-item-' + field + '-set-' + jQuery(this).val()).show();
            }
        });
    });
});